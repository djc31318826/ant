/*
** Name:        threedchartrotation.h
** Purpose:     3D Chart Interactive Rotation
** Author:      Ulrich Telle
** Created:     2021-06-25
** Copyright:   (C) 2021, Ulrich Telle
** License:     LGPL - 3.0 + WITH WxWindows - exception - 3.1
*/

#ifndef THREEDCHARTROTATION_H_
#define THREEDCHARTROTATION_H_

/*!
 * Includes
 */
#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif
#include "wx/file.h"
#include "wxchartviewer.h"
#include "chartdir.h"
#include "wx/textfile.h"
#include "wx/tglbtn.h"
#include "wx/grid.h"
#include <complex>
#include <map>
using namespace std;

enum ID_CONTROL
{
    ID_COMBO_BOX=wxID_HIGHEST+1,
    ID_BUTTON_LOAD,
    ID_BUTTON_SHOW,
    ID_BUTTON_MULTI_MODE,
    ID_BUTTON_RESET_COEF,
    ID_BUTTON_PHI_RANGE,
    ID_TEXT_TH,
    ID_TEXT_PHI,
    ID_TEXT_TH_NUMS,
    ID_TEXT_PHI_NUMS
};

typedef struct __tagAntInfo
{
    int index;
    double x;
    double y;
    double z;
    double phase_load;
    double phase_calc;
} STRUCTANTINFO,*PANTINFO;

/*!
 * Control identifiers
 */

#define SYMBOL_THREEDCHARTROTATION_STYLE wxCAPTION|wxSYSTEM_MENU|wxCLOSE_BOX|wxTAB_TRAVERSAL
#define SYMBOL_THREEDCHARTROTATION_TITLE _("3D Chart Interactive Rotation")
#define SYMBOL_THREEDCHARTROTATION_IDNAME ID_THREEDCHARTROTATION
#define SYMBOL_THREEDCHARTROTATION_SIZE wxSize(400, 300)
#define SYMBOL_THREEDCHARTROTATION_POSITION wxDefaultPosition

/*!
 * ThreeDChartRotation class declaration
 */
class MyCanvas;
extern map<int,STRUCTANTINFO> AntInfo;
#define BEAM_ID_NUMBER 16
class ThreeDChartRotation : public wxFrame
{    
    DECLARE_DYNAMIC_CLASS(ThreeDChartRotation)
    DECLARE_EVENT_TABLE()

public:
  /// Control identifiers
  enum {
    ID_THREEDCHARTROTATION = 10000,
    ID_CHARTVIEWER,
    ID_DRAWFRAME
  };
  wxComboBox *pBox;
    wxTextCtrl *pTextTHNums;
    wxTextCtrl *pTextPhiNums;
    wxTextCtrl *pTextFreq;
    wxTextCtrl *pTextPhiView0;
    wxRadioBox *pRadio;
    wxGrid *pGrid;

    wxTextCtrl *pTextTH[BEAM_ID_NUMBER];
    wxTextCtrl *pTextPhi[BEAM_ID_NUMBER];
    wxTextCtrl *pTextCoefI[BEAM_ID_NUMBER];
    wxTextCtrl *pTextCoefQ[BEAM_ID_NUMBER];
    wxBoxSizer *pSizer[BEAM_ID_NUMBER];
    wxToggleButton *pToggleButton;

    void OnButton1(wxCommandEvent &event);
    void OnButtonLoad(wxCommandEvent &event);
  /// Constructors
  ThreeDChartRotation();
  ThreeDChartRotation( wxWindow* parent, wxWindowID id = SYMBOL_THREEDCHARTROTATION_IDNAME, const wxString& caption = SYMBOL_THREEDCHARTROTATION_TITLE, const wxPoint& pos = SYMBOL_THREEDCHARTROTATION_POSITION, const wxSize& size = SYMBOL_THREEDCHARTROTATION_SIZE, long style = SYMBOL_THREEDCHARTROTATION_STYLE );

  /// Creation
  bool Create( wxWindow* parent, wxWindowID id = SYMBOL_THREEDCHARTROTATION_IDNAME, const wxString& caption = SYMBOL_THREEDCHARTROTATION_TITLE, const wxPoint& pos = SYMBOL_THREEDCHARTROTATION_POSITION, const wxSize& size = SYMBOL_THREEDCHARTROTATION_SIZE, long style = SYMBOL_THREEDCHARTROTATION_STYLE );

  /// Destructor
  ~ThreeDChartRotation();

  /// Initializes member variables
  void Init();

  /// Creates the controls and sizers
  void CreateControls();

  // 3D view angles
  double m_elevationAngle;
  double m_rotationAngle;

  // Keep track of mouse drag
  int m_lastMouseX;
  int m_lastMouseY;
  bool m_isDragging;
  bool MultiMode;
  bool PhiRange;
  MyCanvas *pCanvas;
  // Draw chart, cross section  and crosshair
  void DrawChart();//(wxChartViewer* viewer);

  // Event handler declarations

  void OnExitClick(wxCommandEvent& event);

  /// wxEVT_CLOSE_WINDOW event handler for ID_CROSSHAIR
  void OnCloseWindow(wxCloseEvent& event);
    //void OnBackGround(wxEraseEvent &evt)
    //{

    //}
  void OnClickChart(wxCommandEvent& event);
  void OnMultiMode(wxCommandEvent& event);
  void OnClearCoef(wxCommandEvent& event);
  void OnPhiToggle(wxCommandEvent& event);
  void OnViewPortChanged(wxCommandEvent& event);
  void OnMouseMoveChart(wxCommandEvent& event);
      void UpdateGrid();
  //void OnPaint(wxPaintEvent &event);
  //wxChartViewer* m_chartViewer;
  wxCheckBox* m_drawFrameOnRotate;
};
class MyCanvas:public wxScrolledWindow
{
public:
    bool m_isDragging;
    int origX;
    int origY;
    int m_lastMouseX;
    int m_lastMouseY;
    double m_rotationAngle;
    double m_elevationAngle;

    MyCanvas(wxWindow *parent,
                     wxWindowID winid = wxID_ANY,
                     const wxPoint& pos = wxDefaultPosition,
                     const wxSize& size = wxDefaultSize,
                     long style = wxScrolledWindowStyle,
                     const wxString& name = wxASCII_STR(wxPanelNameStr)):wxScrolledWindow(parent,winid,pos,size,style,name )
                     {

                        SetScrollbars(10,10,200,200,0,0,true);
                        m_isDragging=false;
                        m_rotationAngle=30;
                        m_elevationAngle=30;
                        
                     }
    wxDECLARE_EVENT_TABLE();
    void OnBackGround(wxEraseEvent &evt)
    {

    }


    void OnClick(wxMouseEvent& event);
    void OnMove(wxMouseEvent& event);
    void OnPaint(wxPaintEvent &evt);
    void GetName(wxString &strName)
    {
        while(strName.Right(1)==_(' '))
            strName=strName.Left(strName.length()-1);
        int index=strName.rfind(_(' '));
        if(index!=-1)
        {
            strName=strName.Right(strName.length()-index-1);
        }
    }
/*
    void    ScrollWin(wxScrollWinEvent &evt)
    {
        evt.Skip();
        Refresh();
        //cout<<"dddddddddddddddddddddddddddddddddd"<<endl;
    }
*/
    //virtual void OnDraw(wxDC& dc) wxOVERRIDE;                  
};

#endif
    // THREEDCHARTROTATION_H_
