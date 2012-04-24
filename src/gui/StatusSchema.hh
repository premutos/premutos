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
  enum type_t
  {
    SERVER_STREAMDUP,
    SERVER_MASTERBOX,
    SERVER_STREAMER_RTMP,
    SERVER_STREAMER_HLS,
    SERVER_UNKNOWN,
  };

  enum status_t
  {
    STATUS_WAITING,
    STATUS_INITIALIZING,
    STATUS_RUNNING,
    STATUS_ERROR,
    STATUS_UNKNOWN,
  };

  struct server_t
  {
    unsigned int id;
    std::string hostname;
    wxRect rect;
    enum type_t type;
    enum status_t status;
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

public:
  StatusSchema(wxWindow * parent);

  void addServer(boost::shared_ptr<server_t> server);
  void addLink(const link_t& link);
  void linkAllServers();

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

  void drawServer(wxPaintDC& dc, const boost::shared_ptr<server_t> server);
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
};

}
}

#endif
