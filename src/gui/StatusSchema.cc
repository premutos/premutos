#include "StatusSchema.hh"
#include "../lib/Log.hh"
#include <algorithm>

using namespace livecast::gui;

StatusSchema::StatusSchema(wxWindow * parent)
  : wxControl(parent, wxID_ANY),
    resolution(1280, 1024),
    rectDefSize(resolution.GetWidth() * 0.12, resolution.GetHeight() * 0.2),
    margin(resolution.GetWidth() * 0.02),
    edgeSize(resolution.GetWidth() * 0.007),
    fontSize(resolution.GetHeight() * 0.03)
{
  this->Bind(wxEVT_PAINT, &StatusSchema::refresh, this, wxID_ANY);
  this->Bind(wxEVT_RIGHT_DOWN, &StatusSchema::onOpenPopupMenu, this, wxID_ANY);
  this->Bind(wxEVT_LEFT_DOWN, &StatusSchema::onLeftDown, this, wxID_ANY);
  this->Bind(wxEVT_LEFT_UP, &StatusSchema::onLeftUp, this, wxID_ANY);
}

void StatusSchema::addServer(boost::shared_ptr<StatusSchema::server_t> s)
{

  this->servers.insert(s);
  switch(s->type)
  {
  case SERVER_STREAMDUP: this->computeCoordStreamdup(s); break;
  case SERVER_MASTERBOX: this->computeCoordMasterbox(s); break;
  case SERVER_STREAMER_RTMP: this->computeCoordStreamerRtmp(s); break;
  case SERVER_STREAMER_HLS: this->computeCoordStreamerHls(s); break;
  case SERVER_UNKNOWN: break;
  }

}

void StatusSchema::addLink(const StatusSchema::link_t& l)
{
  this->links.push_back(l);
}

void StatusSchema::linkAllServers()
{
  for (servers_t::iterator itSrc = this->servers.begin(); itSrc != this->servers.end(); ++itSrc)
  {
    if ((*itSrc)->type == SERVER_STREAMDUP)
    {
      link_t l1;
      l1.dsts.push_back(*itSrc);
      this->links.push_back(l1);
      link_t l2;
      for (servers_t::iterator itDst = this->servers.begin(); itDst != this->servers.end(); ++itDst)
      {
        if ((*itDst)->type == SERVER_MASTERBOX)
        {
          l2.src = *itSrc;
          l2.dsts.push_back(*itDst);
          (*itDst)->srcs.push_back(*itSrc);
        }
      }
      this->links.push_back(l2);
    }
    else if ((*itSrc)->type == SERVER_MASTERBOX)
    {
      link_t l;
      for (servers_t::iterator itDst = this->servers.begin(); itDst != this->servers.end(); ++itDst)
      {
        if (((*itDst)->type == SERVER_STREAMER_RTMP) || ((*itDst)->type == SERVER_STREAMER_HLS))
        {
          l.src = *itSrc;
          l.dsts.push_back(*itDst);
          (*itDst)->srcs.push_back(*itSrc);
        }
      }
      this->links.push_back(l);
    }
    else if (((*itSrc)->type == SERVER_STREAMER_RTMP) || ((*itSrc)->type == SERVER_STREAMER_RTMP))
    {
    }
  }
}

void StatusSchema::computeCoordStreamdup(boost::shared_ptr<StatusSchema::server_t> server) const
{
  server->rect.x = this->resolution.GetWidth() * 0.1;
  server->rect.y = (this->resolution.GetHeight() >> 1) - (this->rectDefSize.GetHeight() >> 1);
  server->rect.width = this->rectDefSize.GetWidth();
  server->rect.height = this->rectDefSize.GetHeight();
}

void StatusSchema::computeCoordMasterbox(boost::shared_ptr<StatusSchema::server_t> server) const
{
  unsigned int nbMB = std::count_if(this->servers.begin(), this->servers.end(), 
                                    boost::bind(std::equal_to<type_t>(), boost::bind(&server_t::type, _1), SERVER_MASTERBOX));

  LogError::getInstance().sysLog(DEBUG, "nb masterbox : %u", nbMB);

  server->rect.x = (this->resolution.GetWidth() >> 1) - (this->rectDefSize.GetWidth() >> 1);
  server->rect.width = this->rectDefSize.GetWidth();

  unsigned int nb = (nbMB >> 1) + (((nbMB % 2) == 1) ? 1 : 0) + 1;
  unsigned int yMargin = (this->resolution.GetHeight() >> 1) / nb;
  unsigned int n = 1;
  int pos = 0;
  for (servers_t::iterator it = this->servers.begin(); it != this->servers.end(); ++it)
  {
    if ((*it)->type == SERVER_MASTERBOX)
    { 
      (*it)->rect.height = std::min((int)((yMargin << 1) / 3), this->rectDefSize.GetHeight());
      pos = ((n >> 1) + ((n % 2) == 1 ? 1 : 0)) * (((n % 2) == 1) ? -1 : 1);
      (*it)->rect.y = (this->resolution.GetHeight() >> 1) + (yMargin * pos) - ((*it)->rect.height >> 1);
      n++;
    }
  }

}

void StatusSchema::computeCoordStreamerRtmp(boost::shared_ptr<StatusSchema::server_t> server) const
{
  unsigned int nbSR = std::count_if(this->servers.begin(), this->servers.end(), 
                                    boost::bind(std::equal_to<type_t>(), boost::bind(&server_t::type, _1), SERVER_STREAMER_RTMP));

  LogError::getInstance().sysLog(DEBUG, "nb rtmp streamer : %u", nbSR);

  server->rect.x = this->resolution.GetWidth() - (this->resolution.GetWidth() * 0.1) - this->rectDefSize.GetWidth();
  server->rect.width = this->rectDefSize.GetWidth();

  unsigned int yMargin = (this->resolution.GetHeight() >> 1) / (nbSR + 1);
  unsigned int n = 1;
  for (servers_t::iterator it = this->servers.begin(); it != this->servers.end(); ++it)
  {
    if ((*it)->type == SERVER_STREAMER_RTMP)
    { 
      (*it)->rect.height = std::min((int)((yMargin << 1) / 3), this->rectDefSize.GetHeight());
      (*it)->rect.y = (this->resolution.GetHeight() >> 1) - (yMargin * n) - ((*it)->rect.height >> 1);
      n++;
    }
  }
}

void StatusSchema::computeCoordStreamerHls(boost::shared_ptr<StatusSchema::server_t> server) const
{
  unsigned int nbSH = std::count_if(this->servers.begin(), this->servers.end(), 
                                    boost::bind(std::equal_to<type_t>(), boost::bind(&server_t::type, _1), SERVER_STREAMER_HLS));

  LogError::getInstance().sysLog(DEBUG, "nb hls streamer : %u", nbSH);

  server->rect.x = this->resolution.GetWidth() - (this->resolution.GetWidth() * 0.1) - this->rectDefSize.GetWidth();
  server->rect.width = this->rectDefSize.GetWidth();

  unsigned int yMargin = (this->resolution.GetHeight() >> 1) / (nbSH + 1);
  unsigned int n = 1;
  for (servers_t::iterator it = this->servers.begin(); it != this->servers.end(); ++it)
  {
    if ((*it)->type == SERVER_STREAMER_HLS)
    { 
      (*it)->rect.height = std::min((int)((yMargin << 1) / 3), this->rectDefSize.GetHeight());
      (*it)->rect.y = (this->resolution.GetHeight() >> 1) + (yMargin * n) - ((*it)->rect.height >> 1);
      n++;
    }
  }

}

void StatusSchema::refresh(wxPaintEvent& WXUNUSED(event))
{
  this->hLines.clear();
  this->vLines.clear();

  wxPaintDC dc(this);
  dc.SetBrush(wxNullBrush);

  wxSize size = this->GetSize();
  this->wRatio = (float)(size.GetWidth()) / (float)(this->resolution.GetWidth());
  this->hRatio = (float)(size.GetHeight()) / (float)(this->resolution.GetHeight());
  LogError::getInstance().sysLog(DEBUG, "(%u, %u) => (%u, %u)", this->resolution.GetWidth(), this->resolution.GetHeight(), size.GetWidth(), size.GetHeight());
  LogError::getInstance().sysLog(DEBUG, "wRatio: %f", this->wRatio);
  LogError::getInstance().sysLog(DEBUG, "hRatio: %f", this->hRatio);

  wxSize pixelSize(0, std::max((float)1, this->fontSize * this->hRatio));
  wxFont font(pixelSize, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL);
  dc.SetFont(font);
  dc.SetPen(*wxBLACK_PEN);

  unsigned int n = 1;
  std::for_each(this->links.begin(), this->links.end(),
                boost::bind(&StatusSchema::computeLink, this, _1, n));

  std::for_each(this->servers.begin(), this->servers.end(),
                boost::bind(&StatusSchema::drawServer, this, boost::ref(dc), _1));

  this->drawLines(dc);
}

void StatusSchema::onOpenPopupMenu(wxMouseEvent& WXUNUSED(event))
{
//   LogError::getInstance().sysLog(ERROR, "");
}

void StatusSchema::onLeftDown(wxMouseEvent& WXUNUSED(event))
{
//   LogError::getInstance().sysLog(ERROR, "");
}

void StatusSchema::onLeftUp(wxMouseEvent& WXUNUSED(event))
{
//   LogError::getInstance().sysLog(ERROR, "");
}

void StatusSchema::drawServer(wxPaintDC& dc, const boost::shared_ptr<server_t> server)
{
  wxRect r;
  r.x = server->rect.x * wRatio;
  r.y = server->rect.y * hRatio;
  r.width = server->rect.width * wRatio;
  r.height = server->rect.height * hRatio;
  LogError::getInstance().sysLog(DEBUG, "draw rec(%u, %u, %u, %u) inside (%u, %u, %u, %u)", 
                                 r.x, r.y, r.width, r.height,
                                 0, 0, this->GetSize().GetWidth(), this->GetSize().GetHeight());

  wxBrush brush = dc.GetBrush();
  switch (server->status)
  {
  case STATUS_WAITING:      dc.SetBrush(*wxGREY_BRUSH);   break;
  case STATUS_INITIALIZING: dc.SetBrush(*wxYELLOW_BRUSH); break;
  case STATUS_RUNNING:      dc.SetBrush(*wxGREEN_BRUSH);  break;
  case STATUS_ERROR:        dc.SetBrush(*wxRED_BRUSH);    break;
  case STATUS_UNKNOWN:      dc.SetBrush(*wxRED_BRUSH);    break;
  };

  dc.DrawRectangle(r);

  std::ostringstream title;
  switch (server->type)
  {
  case SERVER_STREAMDUP:     title << "STREAMDUP";     break;
  case SERVER_MASTERBOX:     title << "MASTERBOX";     break;
  case SERVER_STREAMER_RTMP: title << "STREAMER_RTMP"; break;
  case SERVER_STREAMER_HLS:  title << "STREAMER_HLS";  break;
  case SERVER_UNKNOWN:       title << "UNKNOWN TYPE";  break;
  }
  title << "\n" << server->hostname;
  dc.DrawLabel(title.str(), r, wxALIGN_CENTRE);
  dc.SetBrush(brush);
}

void StatusSchema::drawLines(wxPaintDC& dc)
{
  const wxPen pen = dc.GetPen();
  wxPen p(pen);
  for (lines_t::const_iterator hline = this->hLines.begin(); hline != this->hLines.end(); ++hline)
  {
    p.SetColour((*hline).colour);
    p.SetStyle((*hline).style);
    dc.SetPen(p);
    std::list<wxPoint> edges;
    for (lines_t::const_iterator vline = this->vLines.begin(); vline != this->vLines.end(); ++vline)
    {
      StatusSchema::intersect(*hline, *vline, edges);
    }
    if (edges.size() > 0)
    {
      this->drawLineWithEdges(dc, *hline, edges);
    }
    else
    {
      this->drawLine(dc, (*hline).from, (*hline).to);
      if ((*hline).arrow)
      {
        this->drawArrow(dc, (*hline).to, (*hline).from.x < (*hline).to.x);
        this->drawLabel(dc, (*hline).to, (*hline).from, (*hline).label);
      }
    }
  }
  for (lines_t::const_iterator vline = this->vLines.begin(); vline != this->vLines.end(); ++vline)
  {
    p.SetColour((*vline).colour);
    p.SetStyle((*vline).style);
    dc.SetPen(p);
    this->drawLine(dc, (*vline).from, (*vline).to);
  }
  dc.SetPen(pen);
}

void StatusSchema::drawLineWithEdges(wxPaintDC& dc, const line_t& line, const std::list<wxPoint>& edges)
{
  LogError::getInstance().sysLog(DEBUG, "draw line with %u edges", edges.size());

  int way = 1;
  if (line.from.x > line.to.x)
  {
    way = -1;
  }
  
  wxPoint p = *(edges.begin());
  this->drawEdge(dc, p);
  p.x -= (this->edgeSize * way);
  this->drawLine(dc, line.from, p);

  for (std::list<wxPoint>::const_iterator it = edges.begin(); it != edges.end();)
  {
    wxPoint p1 = *it;
    ++it;
    if (it != edges.end())
    {
      wxPoint p2 = *it;
      this->drawEdge(dc, p2);
      p1.x += (this->edgeSize * way);
      p2.x -= (this->edgeSize * way);
      this->drawLine(dc, p1, p2);
    }
  }

  p = *(edges.rbegin());
  this->drawEdge(dc, p);
  p.x += (edgeSize * way);
  this->drawLine(dc, p, line.to);
  this->drawLabel(dc, p, line.to, line.label);
  if (line.arrow)
  {
    this->drawArrow(dc, line.to, p.x < line.to.x);
  }

}

void StatusSchema::drawLine(wxPaintDC& dc, wxPoint p1, wxPoint p2)
{
  p1.x *= this->wRatio;
  p2.x *= this->wRatio;
  
  p1.y *= this->hRatio;
  p2.y *= this->hRatio;

  dc.DrawLine(p1, p2);
}

void StatusSchema::drawArrow(wxPaintDC& dc, wxPoint p, bool way)
{
  int w = (way ? -1 : 1);
  wxPoint p1, p2;
  p1.x = p.x + (3 * w);
  p1.y = p.y - 3;

  p2.x = p.x + (3 * w);
  p2.y = p.y + 3;

  p.x *= this->wRatio;
  p1.x *= this->wRatio;
  p2.x *= this->wRatio;

  p.y *= this->hRatio;
  p1.y *= this->hRatio;
  p2.y *= this->hRatio;

  wxBrush b = dc.GetBrush();
  wxPoint pts[3] = { p, p1, p2 };
  dc.SetBrush(*wxBLACK_BRUSH);
  dc.DrawPolygon(3, pts);
  dc.SetBrush(b);
}

void StatusSchema::drawEdge(wxPaintDC& dc, wxPoint p)
{
  p.x -= this->edgeSize;
  p.y -= this->edgeSize;
  p.x *= this->wRatio;
  p.y *= this->hRatio;
  wxSize arc((this->edgeSize * 2) * this->wRatio, (this->edgeSize * 2) * this->hRatio);
  dc.DrawEllipticArc(p, arc, 180, 0);
}

void StatusSchema::drawLabel(wxPaintDC& dc, const wxPoint& from, const wxPoint& to, const std::string& label)
{
  wxRect r(from.x, from.y - this->fontSize, to.x - from.x, this->fontSize);
  r.x *= this->wRatio;
  r.y *= this->hRatio;
  r.width *= this->wRatio;
  r.height *= this->hRatio;
  dc.DrawLabel(label, r, wxALIGN_CENTRE);
}

void StatusSchema::computeLink(const link_t& link, unsigned int& nb)
{
  for (std::list<boost::shared_ptr<server_t> >::const_iterator dst = link.dsts.begin(); dst != link.dsts.end(); ++dst)
  {

    // special cases
    if (link.src == 0)
    {
      // no source : link from the left or right border
      wxPoint p1, p2;
      std::ostringstream label;
      if ((*dst)->type == SERVER_STREAMDUP)
      {
        p1.x = 0;
        p1.y = (*dst)->rect.y + ((*dst)->rect.GetHeight() >> 1);
        p2.x = (*dst)->rect.x;
        p2.y = p1.y;
        label << (*dst)->protocol << ":" << (*dst)->port;
      }
      else if ((*dst)->type == SERVER_STREAMER_RTMP)
      {
        p1.x = this->resolution.GetWidth();
        p1.y = (*dst)->rect.y + ((*dst)->rect.GetHeight() >> 1);
        p2.x = (*dst)->rect.x + (*dst)->rect.GetWidth();
        p2.y = p1.y;
        label << (*dst)->protocol << ":1935"; // fixme
      }
      else if ((*dst)->type == SERVER_STREAMER_HLS)
      {
        p1.x = this->resolution.GetWidth();
        p1.y = (*dst)->rect.y + ((*dst)->rect.GetHeight() >> 1);
        p2.x = (*dst)->rect.x + (*dst)->rect.GetWidth();
        p2.y = p1.y;
        label << (*dst)->protocol << ":80"; // fixme
      }
      else
      {
        assert(false);
      }
      wxColour colour(0, 0, 0);
      wxPenStyle style = wxPENSTYLE_DOT;
      this->hLines.push_back(line_t(p1, p2, colour, style, true, label.str()));
      continue;
    }

    int xStep = nb * this->margin;
    wxPoint p1, p2, p3, p4;
    wxCoord xSrc, xDst;
    wxCoord ySrc = (link.src->rect.y + (link.src->rect.height >> 1));
    wxCoord yDst = (*dst)->rect.y;
    if (link.src->rect.x < (*dst)->rect.x)
    {
      xSrc = link.src->rect.x + link.src->rect.width;
      xDst = (*dst)->rect.x;
    }
    else
    {
      xStep = -xStep;
      xSrc = link.src->rect.x;
      xDst = (*dst)->rect.x + (*dst)->rect.width;
    }

    assert((*dst)->srcs.size() != 0);
    unsigned int n = (*dst)->srcs.size() + 1;
    unsigned int pos = 1;
    for (std::list<boost::shared_ptr<server_t> >::const_iterator it = (*dst)->srcs.begin(); 
         (it != (*dst)->srcs.end()) && ((*it)->id != link.src->id); ++it)
    {
      pos++;
    }
  
    yDst += (((*dst)->rect.height / n) * pos);

    p1.x = xSrc;
    p1.y = ySrc;

    p2.x = xSrc + xStep;
    p2.y = ySrc;

    p3.x = xSrc + xStep;
    p3.y = yDst;

    p4.x = xDst;
    p4.y = yDst;

    LogError::getInstance().sysLog(DEBUG, "compute line[(%u,%u);(%u,%u);(%u,%u);(%u,%u)]", 
                                   p1.x, p1.y, p2.x, p2.y, p3.x, p3.y, p4.x, p4.y);

    unsigned int r = 150 - (nb * 150 / this->links.size());
    unsigned int g = 0 ;
    unsigned int b = 150 + (nb * 150 / this->links.size());
    wxColour colour(r, g, b);
    wxPenStyle style = (nb % 2 == 1) ? wxPENSTYLE_SOLID : wxPENSTYLE_DOT;
    std::ostringstream label;
    label << (*dst)->protocol << ":" << (*dst)->port;
    this->hLines.push_back(line_t(p1, p2, colour, style, false, ""));
    this->vLines.push_back(line_t(p2, p3, colour, style, false, ""));
    this->hLines.push_back(line_t(p3, p4, colour, style, true, label.str()));

  }
  
  nb++;
}

void StatusSchema::intersect(const line_t& hline, const line_t& vline, std::list<wxPoint>& edges)
{
  // intersection is simple to compute because we know that the first line is horizontal and the second is vertical (otherwise this method doesn't work !!!)
  assert(hline.from.y == hline.to.y);
  assert(vline.from.x == vline.to.x);

  wxPoint p;
  if (((hline.from.x < hline.to.x) && (vline.from.x > hline.from.x) && (vline.from.x < hline.to.x)))
  {
    // p.x = hline.to.x - vline.from.x;
    p.x = vline.from.x;
  }
  else if (((hline.from.x > hline.to.x) && (vline.from.x < hline.from.x) && (vline.from.x > hline.to.x)))
  {
    // p.x = hline.from.x - vline.from.x;
    p.x = vline.from.x;
  }
  if (p.x != 0)
  {
    if ((vline.from.y < vline.to.y) && (hline.from.y > vline.from.y) && (hline.from.y < vline.to.y))
    {
      // p.y = vline.to.y - hline.from.y;
      p.y = hline.from.y;
    }
    else if ((vline.from.y > vline.to.y) && (hline.from.y < vline.from.y) && (hline.from.y > vline.to.y))
    {
      // p.y = vline.from.y - hline.from.y;
      p.y = hline.from.y;
    }
    if (p.y != 0)
    {
      LogError::getInstance().sysLog(DEBUG, "intersection of [(%d,%d);(%d,%d)] and [(%d,%d);(%d,%d)] is (%d,%d)",
                                     hline.from.x, hline.from.y, hline.to.x, hline.to.y,
                                     vline.from.x, vline.from.y, vline.to.x, vline.to.y,
                                     p.x, p.y);

      if (std::find(edges.begin(), edges.end(), p) == edges.end())
      {
        edges.push_back(p);
      }
    }
  }
}
