#ifndef __STATUS_SCHEMA_HH__
#define __STATUS_SCHEMA_HH__

#include <set>
#include <list>
#include <wx/wx.h>
#include <boost/shared_ptr.hpp>
#include <boost/property_tree/ptree.hpp>

namespace livecast {
namespace gui {

class StatusSchema : public wxControl
{
public:

  struct server_t
  {
    unsigned int id;
    std::string hostname;
    wxRect rect;
    wxColour colour;
    unsigned int column;
    std::string type;
    std::string statusDetail;
    std::string protocol;
    unsigned short port;
    bool leaf;
    std::list<boost::shared_ptr<server_t> > srcs;

    bool contained(wxPoint pt, float wRatio, float hRatio) const;
  };

  struct link_t
  {
    boost::shared_ptr<server_t> src;
    std::list<boost::shared_ptr<server_t> > dsts;
  };

  class wxServer : public wxControl
  {
  public:
    wxServer(StatusSchema * parent, 
             boost::shared_ptr<server_t> server,
             wxPoint position,
             wxSize size);

    const boost::shared_ptr<server_t> getInfos() const { return this->infos; }
    void refresh();

  protected:
    void onMouseEnter(wxMouseEvent& event);
    void onMouseLeave(wxMouseEvent& event);
    void OnPaint(wxPaintEvent& event);
    
  private:
    StatusSchema * statusSchema;
    boost::shared_ptr<server_t> infos;
  };

public:
  StatusSchema(wxWindow * parent, unsigned int nbColumn);

  void addServer(boost::shared_ptr<server_t> server);
  void addLink(const link_t& link);
  void linkAllServers();
  void reset();

  const wxSize resolution;
  const wxSize rectDefSize;
  const wxCoord margin;
  const wxCoord edgeSize;
  const unsigned int fontSize;

protected:
  void computeCoordStreamdup(boost::shared_ptr<server_t> server) const;
  void computeCoordMasterbox(boost::shared_ptr<server_t> server) const;
  void computeCoordStreamerRtmp(boost::shared_ptr<server_t> server) const;
  void computeCoordStreamerHls(boost::shared_ptr<server_t> server) const;

  void refresh(wxPaintEvent& event);
  void onOpenPopupMenu(wxMouseEvent& event);
  void onLeftDown(wxMouseEvent& event);
  void onLeftUp(wxMouseEvent& event);
  void onMouseMotion(wxMouseEvent& event);

private:
  struct line_t
  {
    wxPoint from;
    wxPoint to;
    wxColor colour; // fixme : should be an index in a Pen
    wxPenStyle style; // fixme : should be an index in a Pen
    bool arrow;
    const std::string label;
    line_t(wxPoint from, wxPoint to, wxColour& colour, wxPenStyle& style, bool arrow, const std::string& label)
      : from(from), to(to), colour(colour), style(style), arrow(arrow), label(label) {}
  };
  typedef std::list<struct line_t> lines_t;

  struct server_cmp
  {
    bool operator()(const boost::shared_ptr<server_t>& s1, const boost::shared_ptr<server_t>& s2)
      {
        return s1->id < s2->id;
      }
  };

  typedef std::set<boost::shared_ptr<server_t>, struct server_cmp> servers_t;
  typedef std::list<struct link_t> links_t;

  void drawLines(wxPaintDC& dc);
  void drawLineWithEdges(wxPaintDC& dc, const line_t& line, const std::list<wxPoint>& edges);
  void drawLine(wxPaintDC& dc, wxPoint p1, wxPoint p2);
  void drawArrow(wxPaintDC& dc, wxPoint p, bool way);
  void drawEdge(wxPaintDC& dc, wxPoint p);
  void drawLabel(wxPaintDC& dc, const wxPoint& from, const wxPoint& to, const std::string& label);
  void computeLink(const link_t& link, unsigned int& n);
  static void intersect(const line_t& hline, const line_t& vline, std::list<wxPoint>& edges);

  servers_t servers;
  links_t links;
  lines_t hLines;
  lines_t vLines;
  float wRatio;
  float hRatio;
  unsigned int nbColumn;
};

}
}

#endif
