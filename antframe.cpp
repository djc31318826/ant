/*
** Name:        threedchartrotation.cpp
** Purpose:     3D Chart Interactive Rotation
** Author:      Ulrich Telle
** Created:     2021-06-25
** Copyright:   (C) 2021, Ulrich Telle
** License:     LGPL - 3.0 + WITH WxWindows - exception - 3.1
*/

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif
#include <mgl2/qt.h>
#include <mgl2/data.h>
#include <mgl2/fltk.h>
#include <mgl2/glut.h>
#include <complex>
#include <stdio.h>

#include "antframe.h"
#include <wx/dcbuffer.h>
// #include "wxchartviewer.h"

#include <sstream>

using namespace std;

#include "mondrian.xpm"

// ThreeDChartRotation type definition
int th_count = 200;
int ph_count = 200;
double pi = 3.14159;
double th0[BEAM_ID_NUMBER] = {0};
double ph0[BEAM_ID_NUMBER] = {0};
double th0_view = 0;
double ph0_view = 90;
int mode_sel = 0;

double CoefI[BEAM_ID_NUMBER] = {0};
double CoefQ[BEAM_ID_NUMBER] = {0};
map<int, STRUCTANTINFO> AntInfo;
vector<map<int, STRUCTANTINFO>> ArrayInfo;
#if 0
void mgls_prepare3d(mglData *a, mglData *b, mglData *c)
{
    int TH_C = 1000;
    int PH_C = 1000;
    double th;
    double phi;
    //    double *dataX = new double[100 * 100];
    //    double *dataY = new double[100 * 100];
    //    double *dataZ = new double[100 * 100];

    double R = 10;
    int Rows = 8;
    int Cols = 16;

    double x0 = 0;
    double z0 = 0;
    double f = 2.6e9;
    double vc = 3.0e8;
    double pi = 3.14159;
    double lamda = 2 * pi * vc / f;

    double dx = 0.5 * lamda;
    double dz = 0.5 * lamda;

    double *phase;
    complex<double> *gain;

    phase = new double[Rows * Cols];

    // ph=new double[Rows*Cols];

    for (int i = 0; i < Rows; i++)
        for (int j = 0; j < Cols; j++)
        {
            double xi = x0 + j * dx;
            double zi = z0 + i * dz;
            phase[i * Cols + j] = -(xi * sin(th0) * cos(ph0) + zi * cos(th0)) / lamda * 2 * pi;
            // printf("%lf\n", phase[i * Cols + j]);
        }

    a->Create(th_count, ph_count);
    b->Create(th_count, ph_count);
    c->Create(th_count, ph_count);
    gain = new complex<double>[th_count * ph_count];

    double max_gain = 0;
    for (int m = 0; m < th_count; m++)
        for (int n = 0; n < ph_count; n++)
        {
            int i0 = m * ph_count + n;
            gain[m * ph_count + n] = 0;
            double th_iter = pi / th_count * m;
            double ph_iter = 2 * pi / ph_count * n;

            for (int i = 0; i < Rows; i++)
                for (int j = 0; j < Cols; j++)
                {

                    double xi = x0 + j * dx;
                    double zi = z0 + i * dz;
                    double ph_n = (xi * sin(th_iter) * cos(ph_iter) + zi * cos(th_iter)) / lamda * 2 * pi + phase[i * Cols + j];
                    gain[m * ph_count + n] += complex(cos(ph_n), sin(ph_n));
                }
            double abs_gain = abs(gain[m * ph_count + n]);
            a->a[i0] = abs_gain * sin(th_iter) * cos(ph_iter);
            b->a[i0] = abs_gain * sin(th_iter) * sin(ph_iter);
            c->a[i0] = abs_gain * cos(th_iter);
        }
}
#endif
double f = 2.6e9;
bool gMultiMod = false;
bool gPhiRange = false;
double *phase = nullptr;
double max_gain = 0;
double min_gain = 0;
complex<double> *gain = nullptr;
void mgls_prepare3d(mglData *a, mglData *b, mglData *c, mglData *d = nullptr, mglData *e = nullptr)
{
    int TH_C = 1000;
    int PH_C = 1000;
    double th;
    double phi;
    //    double *dataX = new double[100 * 100];
    //    double *dataY = new double[100 * 100];
    //    double *dataZ = new double[100 * 100];

    double R = 10;
    int Rows = 8;
    int Cols = 16;

    double x0 = 0;
    double z0 = 0;

    double vc = 3.0e8;
    double pi = 3.14159;
    double lamda = vc / f;
    printf("lamda=%lf\n", lamda);
    // double dx = 0.5 * lamda;
    // double dz = 0.5 * lamda;

    int ant_total_num = AntInfo.size();
    if (phase == nullptr)
        return;

    // ph=new double[Rows*Cols];

    // for (int i = 0; i < Rows; i++)
    //     for (int j = 0; j < Cols; j++)
    double phase_dims[BEAM_ID_NUMBER];
    complex<double> cSum;
    for (int i = 0; i < ant_total_num; i++)
    {
        // int ant_index;
        // ant_index=AntInfo[i].index;
        cSum = 0;
        for (int j = 0; j < (gMultiMod ? BEAM_ID_NUMBER : 1); j++)
        {
            double xi = AntInfo[i].x;
            double yi = AntInfo[i].y;
            double zi = AntInfo[i].z;
            phase_dims[j] = -(xi * sin(th0[j]) * cos(ph0[j]) + yi * sin(th0[j]) * sin(ph0[j]) + zi * cos(th0[j])) / lamda * 2 * pi;
            // phase_dims[j] 转成复数乘以加权系数后求和
            cSum = cSum + complex<double>(cos(phase_dims[j]), sin(phase_dims[j])) * complex<double>(CoefI[j], CoefQ[j]);
            // phase[i] = -(xi * sin(th0) * cos(ph0) + yi * sin(th0) * sin(ph0) + zi * cos(th0)) / lamda * 2 * pi;
            //  printf("%lf\n", phase[i * Cols + j]);
        }
        // 求cSum的角度，作为phase[i]
        phase[i] = arg(cSum);
    }
    /*
        for (int i = 0; i < ant_total_num; i++)
        {
            // int ant_index;
            // ant_index=AntInfo[i].index;
            for(int j=0;j<)
            double xi = AntInfo[i].x;
            double yi = AntInfo[i].y;
            double zi = AntInfo[i].z;
            phase[i] = -(xi * sin(th0) * cos(ph0) + yi * sin(th0) * sin(ph0) + zi * cos(th0)) / lamda * 2 * pi;
            // printf("%lf\n", phase[i * Cols + j]);
        }
    */
    a->Create(th_count, ph_count);
    b->Create(th_count, ph_count);
    c->Create(th_count, ph_count);
    d->Create(th_count);
    e->Create(th_count);

    if (gain != nullptr)
    {
        delete gain;
        gain = nullptr;
    }
    gain = new complex<double>[th_count * ph_count];
    if (mode_sel == 0)
    {
        for (int m = 0; m < th_count; m++)
            for (int n = 0; n < ph_count; n++)
            {
                int i0 = m * ph_count + n;
                gain[m * ph_count + n] = 0;
                double th_iter = pi / th_count * m;
                double ph_iter = 2 * pi / ph_count * n;
                if (!gPhiRange)
                    ph_iter = ph_iter / 2.0;
                // for (int i = 0; i < Rows; i++)
                //     for (int j = 0; j < Cols; j++)
                for (int i = 0; i < ant_total_num; i++)
                {

                    double xi = AntInfo[i].x;
                    double yi = AntInfo[i].y;
                    double zi = AntInfo[i].z;
                    double ph_n = (xi * sin(th_iter) * cos(ph_iter) + yi * sin(th_iter) * sin(ph_iter) + zi * cos(th_iter)) / lamda * 2 * pi + phase[i];
                    gain[m * ph_count + n] += complex(cos(ph_n), sin(ph_n));
                }
                double abs_gain = abs(gain[m * ph_count + n]);
                gain[m * ph_count + n] = abs_gain;
                if (m == 0 && n == 0)
                {
                    max_gain = min_gain = abs_gain;
                }
                if (abs_gain > max_gain)
                    max_gain = abs_gain;
                if (abs_gain < min_gain)
                    abs_gain = min_gain;

                // a->a[i0] = abs_gain * sin(th_iter) * cos(ph_iter);
                // b->a[i0] = abs_gain * sin(th_iter) * sin(ph_iter);
                // c->a[i0] = abs_gain * cos(th_iter);
            }

        for (int m = 0; m < th_count; m++)
            for (int n = 0; n < ph_count; n++)
            {
                int i0 = m * ph_count + n;
                double th_iter = pi / th_count * m;
                double ph_iter = 2 * pi / ph_count * n;
                if (!gPhiRange)
                    ph_iter = ph_iter / 2.0;
                double abs_gain = abs(gain[i0]);
                a->a[i0] = abs_gain * sin(th_iter) * cos(ph_iter);
                b->a[i0] = abs_gain * sin(th_iter) * sin(ph_iter);
                c->a[i0] = abs_gain * cos(th_iter);
            }
    }
    if (mode_sel == 1)
    {
        complex<double> *gain2 = new complex<double>[th_count];
        if (d != nullptr && e != nullptr)
        {
            for (int m = 0; m < th_count; m++)
            {

                double th_iter = pi / th_count * m;
                double ph_iter = ph0_view / 180 * pi;

                d->a[m] = th_iter / pi * 180; // 转成角度
                // for (int i = 0; i < Rows; i++)
                //     for (int j = 0; j < Cols; j++)
                for (int i = 0; i < ant_total_num; i++)
                {

                    double xi = AntInfo[i].x;
                    double yi = AntInfo[i].y;
                    double zi = AntInfo[i].z;
                    double ph_n = (xi * sin(th_iter) * cos(ph_iter) + yi * sin(th_iter) * sin(ph_iter) + zi * cos(th_iter)) / lamda * 2 * pi + phase[i];
                    gain2[m] += complex(cos(ph_n), sin(ph_n));
                }
                double abs_gain = abs(gain2[m]);
                gain2[m] = abs_gain;
                if (m == 0)
                {
                    max_gain = min_gain = abs_gain;
                }
                if (abs_gain > max_gain)
                    max_gain = abs_gain;
                if (abs_gain < min_gain)
                    abs_gain = min_gain;
            }
            for (int m = 0; m < th_count; m++)
            {
                double abs_gain = abs(gain2[m]);
                if (abs_gain < 1e-10)
                {
                    e->a[m] = -100;
                }
                else
                {
                    e->a[m] = 10 * log10(abs_gain / max_gain);
                }
            }
        }
        // delete gain;
        delete gain2;
    }
}

//////////////////
int smgl_surf3(mglGraph *gr)
{
    mglData a, b, c, d, e; //,d;
    mglData xx, yy, zz,rr;
    // if(big!=3)
    //{	gr->SubPlot(2,2,0);	gr->Title("Surf3 plot (default)");	}
    if (mode_sel == 0)
    {
        mgls_prepare3d(&a, &b, &c, &d, &e);
        gr->Rotate(30, 40);
        // printf("%lf,%lf\n",m_rotationAngle,m_elevationAngle);
        gr->Light(true);
        gr->Alpha(true);
        int total_ant_num = AntInfo.size();
        gr->SetRange('x', -total_ant_num, total_ant_num);
        gr->SetRange('y', -total_ant_num, total_ant_num);
        gr->SetRange('z', -total_ant_num, total_ant_num);
        gr->Label('x', "X", 0);
        gr->Label('y', "Y", 0);
        gr->Label('z', "Z", 0);
        gr->Axis();
        printf("total_ant_num=%d\n", total_ant_num);

        // 绘制天线并获取x,y,z的中心点
        /*
            mglData a1, b1, c1; //,d;
            a1.Create(total_ant_num);
            b1.Create(total_ant_num);
            c1.Create(total_ant_num);

            for(int i=0;i<total_ant_num;i++)
            {
                a1.a[i]=AntInfo[i].x*1000;
                b1.a[i]=AntInfo[i].y*1000;
                c1.a[i]=AntInfo[i].z*1000;
            }
        */
        // gr->Dots(a1, b1, c1);

        //  gr->Surf3(c);
        // gr->SubPlot(3,1,0);
        gr->Mesh(a, b, c);
    }
    if (mode_sel == 1)
    {
        mgls_prepare3d(&a, &b, &c, &d, &e);
        gr->Rotate(0, 0);

        gr->Label('x', "TH", 0);
        gr->Label('y', "Gain", 0);

        gr->SetRange('x', 0, 180);
        gr->SetRange('y', -30, 0);
        wxString str;
        if (max_gain > 1e-10)
        {
            str = wxString::Format(wxT("ant gain:%.2lfdB"), 10 * log10(max_gain));
        }
        else
        {
            str = wxT("ant gain:NA");
        }
        const char *p = str.c_str().AsChar();
        gr->Title(p);
        gr->Axis();
        gr->Plot(d, e);
    }

    if (mode_sel == 2)
    {
        gr->Rotate(0, 0);

        gr->Label('x', "X", 0);
        gr->Label('y', "Z", 0);
        //gr->Label('z', "z", 0);

        gr->Axis();

        int tSize = AntInfo.size();
        xx.Create(tSize);
        yy.Create(tSize);
        zz.Create(tSize);
        rr.Create(tSize);

        int minx, maxx, miny, maxy, minz, maxz;

        for (int i = 0; i < tSize; i++)
        {
            if (i == 0)
            {
                minx = maxx = AntInfo[i].x;
                miny = maxy = AntInfo[i].y;
                minz = maxz = AntInfo[i].z;
            }
            if (minx > AntInfo[i].x)
                minx = AntInfo[i].x;
            if (maxx < AntInfo[i].x)
                maxx = AntInfo[i].x;

            if (miny > AntInfo[i].y)
                miny = AntInfo[i].y;
            if (maxy < AntInfo[i].y)
                maxy = AntInfo[i].y;
            if (minz > AntInfo[i].z)
                minz = AntInfo[i].z;
            if (maxz < AntInfo[i].z)
                maxz = AntInfo[i].z;
            xx.a[i] = AntInfo[i].x;
            yy.a[i] = AntInfo[i].y;
            zz.a[i] = AntInfo[i].z;
            rr.a[i] =1 ;

        }

        gr->SetRange('x', minx, maxx);
        gr->SetRange('y', miny, maxy);
        //gr->SetRange('z', minz, maxz);

        gr->Mark(xx, zz,rr,"s");
    }
    // gr->Dots(a,b,c);
    //  gr->WriteJPEG("./mathgl.jpg");

    // gr->Surf3(d);
    return 0;
    // if(big==3)	return;
    // gr->SubPlot(2,2,1);	gr->Title("'\\#' style");
    // gr->Rotate(50,60);	gr->Box();	gr->Surf3(c,"#");
    // gr->SubPlot(2,2,2);	gr->Title("'.' style");
    // gr->Rotate(50,60);	gr->Box();	gr->Surf3(c,".");
}

int smgl_surf3(mglGraph *gr, double m_rotationAngle, double m_elevationAngle)
{
    mglData a, b, c; //,d;

    mgls_prepare3d(&a, &b, &c);
    // if(big!=3)
    //{	gr->SubPlot(2,2,0);	gr->Title("Surf3 plot (default)");	}
    gr->Rotate(m_elevationAngle, m_rotationAngle);
    // printf("%lf,%lf\n",m_rotationAngle,m_elevationAngle);
    gr->Light(true);
    gr->Alpha(true);
    gr->SetRange('x', -128, 128);
    gr->SetRange('y', -128, 128);
    gr->SetRange('z', -128, 128);
    gr->Label('x', "X", 0);
    gr->Label('y', "Y", 0);
    gr->Label('z', "Z", 0);
    gr->Axis();
    //  gr->Surf3(c);
    gr->Mesh(a, b, c);

    // gr->WriteJPEG("./mathgl.jpg");

    // gr->Surf3(d);
    return 0;
    // if(big==3)	return;
    // gr->SubPlot(2,2,1);	gr->Title("'\\#' style");
    // gr->Rotate(50,60);	gr->Box();	gr->Surf3(c,"#");
    // gr->SubPlot(2,2,2);	gr->Title("'.' style");
    // gr->Rotate(50,60);	gr->Box();	gr->Surf3(c,".");
}

wxBEGIN_EVENT_TABLE(MyCanvas, wxScrolledWindow)
    EVT_ERASE_BACKGROUND(MyCanvas::OnBackGround)
    // EVT_PAINT(MyCanvas::OnPaint)
    // EVT_LEFT_DOWN(MyCanvas::OnClick)
    // EVT_MOTION(MyCanvas::OnMove)
    // EVT_SCROLLWIN(MyCanvas::ScrollWin)
    wxEND_EVENT_TABLE() void MyCanvas::OnClick(wxMouseEvent &event)
{
    if (m_isDragging == true && !wxGetMouseState().ButtonIsDown(wxMOUSE_BTN_ANY))
        m_isDragging = false;
    origX = event.GetPosition().x;
    origY = event.GetPosition().y;
    m_lastMouseX = origX;
    m_lastMouseY = origY;
}
void MyCanvas::OnMove(wxMouseEvent &event)
{
    if (m_isDragging == true && !wxGetMouseState().ButtonIsDown(wxMOUSE_BTN_ANY))
        m_isDragging = false;

    m_lastMouseX = event.GetPosition().x;
    m_lastMouseY = event.GetPosition().y;

    // Drag occurs if mouse is moving and mouse button is down
    if (wxGetMouseState().ButtonIsDown(wxMOUSE_BTN_ANY))
    {
        if (m_isDragging)
        {
            // The chart is configured to rotate by 90 degrees when the mouse moves from
            // left to right, which is the plot region width (360 pixels). Similarly, the
            // elevation changes by 90 degrees when the mouse moves from top to buttom,
            // which is the plot region height (270 pixels).
            m_rotationAngle += (m_lastMouseX - origX) / 100;
            m_elevationAngle += (origY - m_lastMouseY) / 100;
            // m_chartViewer->updateViewPort(true, false);
        }

        // Keep track of the last mouse position
        // m_lastMouseX = origX;
        // m_lastMouseY = origY;
        m_isDragging = true;
        Refresh();
    }
    else if (m_isDragging)
    {
        // mouse up means not dragging
        m_isDragging = false;
        // chartViewer->updateViewPort(true, false);
    }
}
void MyCanvas::OnPaint(wxPaintEvent &evt)
{
    wxBufferedPaintDC dcOrig(this);
    PrepareDC(dcOrig);
    dcOrig.Clear();
    dcOrig.SetFont(wxFontInfo(10).Family(wxFONTFAMILY_MODERN));
    dcOrig.SetBackgroundMode(wxBRUSHSTYLE_TRANSPARENT);
    dcOrig.SetPen(*wxBLACK_PEN);
    // dcOrig.SetBackground(*wxWHITE_BRUSH);
    dcOrig.SetBackground(*wxYELLOW_BRUSH);
    dcOrig.SetBrush(*wxWHITE_BRUSH);
    dcOrig.DrawText("weqerwqer", 0, 0);

    int w, h, x, y = 0;
    GetClientSize(&w, &h); // size of the picture
    mglGraph gr(0, w, h);

    gr.Alpha(true); // draws something using MathGL
    gr.Light(true);
    smgl_surf3(&gr, m_rotationAngle, m_elevationAngle);

    wxImage img(w, h, (unsigned char *)gr.GetRGB(), true);
    dcOrig.DrawBitmap(wxBitmap(img), 0, y);
    // img.SaveFile("./test3.jpg");
    // ToolBar->GetSize(&x,&y);    // gets a height of the toolbar if any
    // wxPaintDC dc(this);         // and draws it
}

IMPLEMENT_DYNAMIC_CLASS(ThreeDChartRotation, wxFrame)

// CrossHair event table definition

BEGIN_EVENT_TABLE(ThreeDChartRotation, wxFrame)

EVT_MENU(wxID_EXIT, ThreeDChartRotation::OnExitClick)
EVT_CLOSE(ThreeDChartRotation::OnCloseWindow)
// EVT_ERASE_BACKGROUND(ThreeDChartRotation::OnBackGround)
// EVT_CHARTVIEWER_CLICKED(ID_CHARTVIEWER, ThreeDChartRotation::OnClickChart)
// EVT_CHARTVIEWER_MOUSEMOVE_CHART(ID_CHARTVIEWER, ThreeDChartRotation::OnMouseMoveChart)
// EVT_CHARTVIEWER_VIEWPORT_CHANGED(ID_CHARTVIEWER, ThreeDChartRotation::OnViewPortChanged)
// EVT_PAINT(OnPaint)
END_EVENT_TABLE()

// ThreeDChartRotation constructors

/*
void ThreeDChartRotation::OnPaint(wxPaintEvent &event)
{

    int w,h,x,y=0;
  GetClientSize(&w,&h);   // size of the picture
  mglGraph gr(0,w,h);

  gr.Alpha(true);         // draws something using MathGL
  gr.Light(true);
  smgl_surf3(&gr);

  wxImage img(w,h,(unsigned char *)gr.GetRGB(),true);
  img.SaveFile("./test3.jpg");
  //ToolBar->GetSize(&x,&y);    // gets a height of the toolbar if any
  wxPaintDC dc(this);         // and draws it
  dc.SetPen(*wxBLACK_PEN);
  dc.DrawBitmap(wxBitmap(img),0,y);
  dc.DrawText("dfadfsadf",0,0);
   // event.Skip();
}
*/
ThreeDChartRotation::ThreeDChartRotation()
{
    Init();
}

ThreeDChartRotation::ThreeDChartRotation(wxWindow *parent, wxWindowID id, const wxString &caption, const wxPoint &pos, const wxSize &size, long style)
{
    Init();
    Create(parent, id, caption, pos, size, style);
}

// ThreeDChartRotation creator
bool ThreeDChartRotation::Create(wxWindow *parent, wxWindowID id, const wxString &caption, const wxPoint &pos, const wxSize &size, long style)
{
    SetExtraStyle(wxWS_EX_VALIDATE_RECURSIVELY);
    wxFrame::Create(parent, id, caption, pos, size, style);

    CreateControls();
    if (GetSizer())
    {
        GetSizer()->SetSizeHints(this);
    }
    Centre();

    return true;
}

// ThreeDChartRotation destructor
ThreeDChartRotation::~ThreeDChartRotation()
{
    // delete m_chartViewer->getChart();
    if (gain != nullptr)
    {
        delete gain;
        gain = nullptr;
    }

    if (phase != nullptr)
    {
        delete phase;
        phase = nullptr;
    }
}

// Member initialisation
void ThreeDChartRotation::Init()
{
    // m_chartViewer = NULL;
    MultiMode = false;
    gMultiMod = MultiMode;

    PhiRange = false;
    gPhiRange = PhiRange;
}

// Control creation for ThreeDChartRotation
void ThreeDChartRotation::CreateControls()
{
    wxString str;
    SetIcon(wxICON(mondrian));
    ThreeDChartRotation *itemDialog1 = this;

    SetBackgroundColour(*wxWHITE);

    wxBoxSizer *itemBoxSizer2 = new wxBoxSizer(wxVERTICAL);
    itemDialog1->SetSizer(itemBoxSizer2);

    wxBoxSizer *itemBoxSizer3 = new wxBoxSizer(wxHORIZONTAL);
    itemBoxSizer2->Add(itemBoxSizer3, 0, wxALIGN_LEFT | wxLEFT, FromDIP(15));
    // m_drawFrameOnRotate = new wxCheckBox(this, ID_DRAWFRAME, _("Draw Frame on Rotate"), wxDefaultPosition, wxDefaultSize, 0);
    // m_drawFrameOnRotate->SetValue(true);
    // itemBoxSizer3->Add(m_drawFrameOnRotate, 0, wxALIGN_LEFT | wxALL, FromDIP(3));
    // 列表显示当前load的坐标，按钮 load坐标，按键 绘图
    // 静态文本 输入角度th 文本框1，静态文本 输入角度phi 文本框2，
    // 静态文本 输入角度th分段 文本框1，静态文本 输入角度phi段 文本框2
    /*ID_COMBO_BOX,
    ID_BUTTON_LAOD,
    ID_BUTTON_SHOW,
    ID_TEXT_TH,
    ID_TEXT_PHI,
    ID_TEXT_TH_NUMS,
    ID_TEXT_PHI_NUMS
    */

    pBox = new wxComboBox(this, ID_COMBO_BOX,wxEmptyString,wxDefaultPosition,FromDIP(wxSize(220,20)));

    wxButton *pButton = new wxButton(this, ID_BUTTON_LOAD, wxT("读取坐标"));
    pButton->Bind(wxEVT_BUTTON, &ThreeDChartRotation::OnButtonLoad, this, ID_BUTTON_LOAD);
    itemBoxSizer3->Add(pBox, 1, wxALIGN_LEFT | wxALL | wxEXPAND, FromDIP(3));
    itemBoxSizer3->Add(pButton, 0, wxALIGN_LEFT | wxALL, FromDIP(3));
    pButton = new wxButton(this, ID_BUTTON_SHOW, wxT("显示图形"));
    pButton->Bind(wxEVT_BUTTON, &ThreeDChartRotation::OnButton1, this, ID_BUTTON_SHOW);
    itemBoxSizer3->Add(pButton, 0, wxALIGN_LEFT | wxALL, FromDIP(3));

    pButton = new wxButton(this, ID_BUTTON_MULTI_MODE, wxT("多基模式"));
    pButton->Bind(wxEVT_BUTTON, &ThreeDChartRotation::OnMultiMode, this, ID_BUTTON_MULTI_MODE);
    itemBoxSizer3->Add(pButton, 0, wxALIGN_LEFT | wxALL, FromDIP(3));

    pButton = new wxButton(this, ID_BUTTON_RESET_COEF, wxT("系数复位"));
    pButton->Bind(wxEVT_BUTTON, &ThreeDChartRotation::OnClearCoef, this, ID_BUTTON_RESET_COEF);
    itemBoxSizer3->Add(pButton, 0, wxALIGN_LEFT | wxALL, FromDIP(3));
    pToggleButton = new wxToggleButton(this, ID_BUTTON_PHI_RANGE, wxT("PHI扫描范围180度"));
    pToggleButton->Bind(wxEVT_TOGGLEBUTTON, &ThreeDChartRotation::OnPhiToggle, this, ID_BUTTON_PHI_RANGE);
    itemBoxSizer3->Add(pToggleButton, 0, wxALIGN_LEFT | wxALL, FromDIP(3));

    wxArrayString strArray;
    strArray.Add(wxT("三维方向图"));
    strArray.Add(wxT("二维切面方向图"));
    strArray.Add(wxT("阵子坐标图"));

    wxBoxSizer *itemBoxSizer0 = new wxBoxSizer(wxHORIZONTAL);
    itemBoxSizer2->Add(itemBoxSizer0, 0, wxALIGN_LEFT | wxLEFT, FromDIP(15));
    pRadio = new wxRadioBox(this, wxID_ANY, wxT("绘图选择"), wxDefaultPosition, wxDefaultSize, strArray);
    pRadio->SetSelection(0);
    itemBoxSizer0->Add(pRadio, 0, wxALIGN_LEFT | wxALL, FromDIP(3));

    for (int i = 0; i < BEAM_ID_NUMBER; i++)
    {

        pSizer[i] = new wxBoxSizer(wxHORIZONTAL);
        itemBoxSizer2->Add(pSizer[i], 0, wxALIGN_LEFT | wxLEFT, FromDIP(15));

        str = wxString::Format(wxT("Th设置%02d"), i);
        pSizer[i]->Add(new wxStaticText(this, wxID_ANY, str), 0, wxALIGN_LEFT | wxALL, FromDIP(3));

        pTextTH[i] = new wxTextCtrl(this, ID_TEXT_TH, wxT("90"));
        pSizer[i]->Add(pTextTH[i], 0, wxALIGN_LEFT | wxALL, FromDIP(3));
        pTextPhi[i] = new wxTextCtrl(this, ID_TEXT_PHI, wxT("90"));
        str = wxString::Format(wxT("Phi设置%02d"), i);
        pSizer[i]->Add(new wxStaticText(this, wxID_ANY, str), 0, wxALIGN_LEFT | wxALL, FromDIP(3));
        pSizer[i]->Add(pTextPhi[i], 0, wxALIGN_LEFT | wxALL, FromDIP(3));

        pSizer[i]->Add(new wxStaticText(this, wxID_ANY, wxT("系数I：")), 0, wxALIGN_LEFT | wxALL, FromDIP(3));
        if (i == 0)
            pTextCoefI[i] = new wxTextCtrl(this, ID_TEXT_TH, wxT("7ff"));
        else
            pTextCoefI[i] = new wxTextCtrl(this, ID_TEXT_TH, wxT("0"));
        pSizer[i]->Add(pTextCoefI[i], 0, wxALIGN_LEFT | wxALL, FromDIP(3));

        pTextCoefQ[i] = new wxTextCtrl(this, ID_TEXT_TH, wxT("0"));
        pSizer[i]->Add(new wxStaticText(this, wxID_ANY, wxT("系数Q：")), 0, wxALIGN_LEFT | wxALL, FromDIP(3));
        pSizer[i]->Add(pTextCoefQ[i], 0, wxALIGN_LEFT | wxALL, FromDIP(3));
    }
    wxBoxSizer *itemBoxSizer4 = new wxBoxSizer(wxHORIZONTAL);

    itemBoxSizer4->Add(new wxStaticText(this, wxID_ANY, wxT("Th分段数    ")), 0, wxALIGN_LEFT | wxALL, FromDIP(3));

    pTextTHNums = new wxTextCtrl(this, ID_TEXT_TH_NUMS, wxT("100"));
    itemBoxSizer4->Add(pTextTHNums, 0, wxALIGN_LEFT | wxALL, FromDIP(3));
    pTextPhiNums = new wxTextCtrl(this, ID_TEXT_PHI_NUMS, wxT("100"));
    itemBoxSizer4->Add(new wxStaticText(this, wxID_ANY, wxT("Phi分段数")), 0, wxALIGN_LEFT | wxALL, FromDIP(3));
    itemBoxSizer4->Add(pTextPhiNums, 0, wxALIGN_LEFT | wxALL, FromDIP(3));

    itemBoxSizer2->Add(itemBoxSizer4, 0, wxALIGN_LEFT | wxLEFT, FromDIP(15));

    wxBoxSizer *itemBoxSizer5 = new wxBoxSizer(wxHORIZONTAL);

    itemBoxSizer5->Add(new wxStaticText(this, wxID_ANY, wxT("频率    ")), 0, wxALIGN_LEFT | wxALL, FromDIP(3));
    pTextFreq = new wxTextCtrl(this, ID_TEXT_PHI_NUMS, wxT("26000"));
    itemBoxSizer5->Add(pTextFreq, 0, wxALIGN_LEFT | wxALL, FromDIP(3));
    itemBoxSizer5->Add(new wxStaticText(this, wxID_ANY, wxT("MHz   ")), 0, wxALIGN_LEFT | wxALL, FromDIP(3));
    itemBoxSizer2->Add(itemBoxSizer5, 0, wxALIGN_LEFT | wxLEFT, FromDIP(15));

    wxBoxSizer *itemBoxSizer6 = new wxBoxSizer(wxHORIZONTAL);
    itemBoxSizer6->Add(new wxStaticText(this, wxID_ANY, wxT("选取PHI角度    ")), 0, wxALIGN_LEFT | wxALL, FromDIP(3));
    pTextPhiView0 = new wxTextCtrl(this, ID_TEXT_PHI_NUMS, wxT("90"));
    itemBoxSizer6->Add(pTextPhiView0, 0, wxALIGN_LEFT | wxALL, FromDIP(3));
    itemBoxSizer6->Add(new wxStaticText(this, wxID_ANY, wxT("单位度   ")), 0, wxALIGN_LEFT | wxALL, FromDIP(3));
    itemBoxSizer2->Add(itemBoxSizer6, 0, wxALIGN_LEFT | wxLEFT, FromDIP(15));

    wxBoxSizer *itemBoxSizer7 = new wxBoxSizer(wxHORIZONTAL);
    pGrid = new wxGrid(this, wxID_ANY, wxDefaultPosition, wxDefaultSize);
    pGrid->CreateGrid(8, 8);
    itemBoxSizer7->Add(pGrid, 1, wxALIGN_LEFT | wxALL, FromDIP(3));
    itemBoxSizer2->Add(itemBoxSizer7, 0, wxALIGN_LEFT | wxLEFT, FromDIP(15));

    // pCanvas=new MyCanvas(this,wxID_ANY,wxDefaultPosition,wxSize(800,600));
    // itemBoxSizer1->Add(pCanvas, 0, wxALIGN_CENTER_VERTICAL | wxTOP, FromDIP(5));
    //  Main Chart Viewer
    // m_chartViewer = new wxChartViewer(this, ID_CHARTVIEWER, wxDefaultPosition, FromDIP(wxSize(720, 600)));
    // itemDialog1->SetSizerAndFit
    //  3D view angles

    // Keep track of mouse drag
    m_isDragging = false;
    m_lastMouseX = -1;
    m_lastMouseY = -1;

    for (int i = 1; i < BEAM_ID_NUMBER; i++)
    {
        pSizer[i]->Show(MultiMode);
    }
    // Fit();
    // Refresh();
    Layout();
    pGrid->FitInside();
    // Update the viewport to display the chart
    // m_chartViewer->updateViewPort(true, false);
}

// Draw chart
void ThreeDChartRotation::DrawChart()
{
}
#if 0
void ThreeDChartRotation::DrawChart(wxChartViewer *viewer)
{
    // The x and y coordinates of the grid
    /*
    double dataX[] = { -10, -9, -8, -7, -6, -5, -4, -3, -2, -1, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
    const int dataXLength = (int)(sizeof(dataX) / sizeof(*dataX));

    double dataY[] = { -10, -9, -8, -7, -6, -5, -4, -3, -2, -1, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
    const int dataYLength = (int)(sizeof(dataY) / sizeof(*dataY));
    */
    double th;
    double phi;
    //    double *dataX = new double[100 * 100];
    //    double *dataY = new double[100 * 100];
    //    double *dataZ = new double[100 * 100];

    double R = 10;

    int Rows = 8;
    int Cols = 16;

    double x0 = 0;
    double z0 = 0;
    double f = 2.6e9;
    double vc = 3.0e8;

    double lamda = 2 * pi * vc / f;

    double dx = 0.5 * lamda;
    double dz = 0.5 * lamda;

    double th0 = 30;
    double ph0 = 90;

    th0 = th0 / 180 * pi;
    ph0 = ph0 / 180 * pi;

    double *phase;
    complex<double> *gain;

    phase = new double[Rows * Cols];

    // ph=new double[Rows*Cols];

    for (int i = 0; i < Rows; i++)
        for (int j = 0; j < Cols; j++)
        {
            double xi = x0 + j * dx;
            double zi = z0 + i * dz;
            phase[i * Cols + j] = -(xi * sin(th0) * cos(ph0) + zi * cos(th0)) / lamda * 2 * pi;
            printf("%lf\n", phase[i * Cols + j]);
        }
    int th_count = 100;
    int ph_count = 100;
    gain = new complex<double>[th_count * ph_count];
    double *dataX = new double[th_count * ph_count];
    double *dataY = new double[th_count * ph_count];
    double *dataZ = new double[th_count * ph_count*100*100];
    memset(dataZ,0,sizeof(double)*th_count * ph_count*100*100);
    for (int i = 0; i < th_count * ph_count; i++)
    {
        dataX[i] = dataY[i] = dataZ[i] = 0;
    }

    double max_gain = 0;
    for (int m = 0; m < th_count; m++)
        for (int n = 0; n < ph_count; n++)
        {
            gain[m * ph_count + n] = 0;
            double th_iter = pi / th_count * m;
            double ph_iter = 2 * pi / ph_count * n / 2;

            for (int i = 0; i < Rows; i++)
                for (int j = 0; j < Cols; j++)
                {

                    double xi = x0 + j * dx;
                    double zi = z0 + i * dz;
                    double ph_n = (xi * sin(th_iter) * cos(ph_iter) + zi * cos(th_iter)) / lamda * 2 * pi + phase[i * Cols + j];
                    gain[m * ph_count + n] += complex(cos(ph_n), sin(ph_n));
                }
            double abs_gain = abs(gain[m * ph_count + n]);
            dataX[m * ph_count + n] = abs_gain * sin(th_iter) * cos(ph_iter);
            dataY[m * ph_count + n] = abs_gain * sin(th_iter) * sin(ph_iter);
            dataZ[m * ph_count + n] = abs_gain * cos(th_iter);

            // printf("%lf",abs_gain);
            if (max_gain < abs_gain)
                max_gain = abs_gain;
        }

    /*
for(int k=0;k<500;k++)
{
    printf("%d,%lf:%lf:%lf\n",k,dataX[k],dataY[k],dataZ[k]);
}
*/
    wxFile file1("./ant_data.txt", wxFile::OpenMode::write);

    for (int k = 0; k < th_count * ph_count; k++)
    {
        // printf("%d,%lf:%lf:%lf\n",k,dataX[k],dataY[k],dataZ[k]);
        file1.Write(wxString::Format("%lf %lf %lf\n", dataX[k], dataY[k], dataZ[k]));
    }

    file1.Close();

    delete phase;

    /*
    for (int i = 0; i < 50; i++)
    {
        th = i * 3.14159 / 100;
        for (int j = 0; j < 100; j++)
        {
            phi = j * 2 * 3.14159 / 100;

            dataX[i * 100 + j] = R * sin(th) * cos(phi);
            dataY[i * 100 + j] = R * sin(th) * sin(phi);
            dataZ[i * 100 + j] = R * cos(th);

            if(dataZ[i * 100 + j]>100)
            {
                dataZ[i * 100 + j]=0;
            }

        }
    }
    */

    // The values at the grid points. In this example, we will compute the values using the
    // formula z = x * sin(y) + y * sin(x).
    /*
    double dataZ[dataXLength * dataYLength];
    for (int yIndex = 0; yIndex < dataYLength; ++yIndex)
    {
        double y = dataY[yIndex];
        for (int xIndex = 0; xIndex < dataXLength; ++xIndex)
        {
            double x = dataX[xIndex];
            dataZ[yIndex * dataXLength + xIndex] = x * sin(y) + y * sin(x);
        }
    }
    */
    int dataXLength = th_count * ph_count;
    int dataYLength = th_count * ph_count;
    int dataZLength = th_count * ph_count;

    // Create a SurfaceChart object of size 720 x 600 pixels
#define SURFACE
#ifdef SURFACE
    SurfaceChart *c = new SurfaceChart(720, 600);
#else
    ThreeDScatterChart *c = new ThreeDScatterChart(720, 600);
#endif
    // Set the center of the plot region at (330, 290), and set width x depth x height to
    // 360 x 360 x 270 pixels
    c->setPlotRegion(330, 290, 360, 360, 270);

    // Set the data to use to plot the chart
//    c->setData(DoubleArray(dataX, dataXLength), DoubleArray(dataY, dataYLength),
//               DoubleArray(dataZ, dataZLength));

#ifdef SURFACE
    c->setData(DoubleArray(dataX, dataXLength), DoubleArray(dataY, dataXLength),
               DoubleArray(dataZ, dataXLength));
#else
    c->addScatterGroup(DoubleArray(dataX, dataXLength), DoubleArray(dataY, dataYLength),
                       DoubleArray(dataZ, dataZLength), "", Chart::GlassSphere2Shape, 11, Chart::SameAsMainColor);
#endif
    // Spline interpolate data to a 80 x 80 grid for a smooth surface
    // c->setInterpolation(80, 80);

    // Set the view angles
    c->setViewAngle(m_elevationAngle, m_rotationAngle);

// Check if draw frame only during rotation
#ifdef SURFACE
    if (m_isDragging && m_drawFrameOnRotate->IsChecked())
        c->setShadingMode(Chart::RectangularFrame);
#endif
    // Add a color axis (the legend) in which the left center is anchored at (660, 270). Set
    // the length to 200 pixels and the labels on the right side.
    c->setColorAxis(650, 270, Chart::Left, 200, Chart::Right);

    // Set the x, y and z axis titles using 10 points Arial Bold font
    c->xAxis()->setTitle("X", "Arial Bold", 15);
    c->yAxis()->setTitle("Y", "Arial Bold", 15);
    c->zAxis()->setTitle("Z", "Arial Bold", 15);

    // Set axis label font
    c->xAxis()->setLabelStyle("Arial", 10);
    c->yAxis()->setLabelStyle("Arial", 10);
    c->zAxis()->setLabelStyle("Arial", 10);
    c->colorAxis()->setLabelStyle("Arial", 10);

    // Output the chart
    delete viewer->getChart();
    viewer->setChart(c);

    // include tool tip for the chart
    viewer->setImageMap(c->getHTMLImageMap("", "",
                                           "title='<*cdml*>X: {x|2}<*br*>Y: {y|2}<*br*>Z: {z|2}'"));
}
#endif
void ThreeDChartRotation::OnViewPortChanged(wxCommandEvent &event)
{
    // Update chart if necessary
    // if (m_chartViewer->needUpdateChart())
    //{
    //    DrawChart(m_chartViewer);
    //}
}

void ThreeDChartRotation::OnClickChart(wxCommandEvent &event)
{

    if (m_isDragging && !wxGetMouseState().ButtonIsDown(wxMOUSE_BTN_ANY))
    {
        // mouse up means not dragging
        m_isDragging = false;
        // m_chartViewer->updateViewPort(true, false);
    }
}

// Draw track cursor when mouse is moving over plotarea
void ThreeDChartRotation::OnMouseMoveChart(wxCommandEvent &evt)
{
    /*int mouseX = m_chartViewer->getChartMouseX();
    int mouseY = m_chartViewer->getChartMouseY();

    // Drag occurs if mouse is moving and mouse button is down
    if (wxGetMouseState().ButtonIsDown(wxMOUSE_BTN_ANY))
    {
        if (m_isDragging)
        {
            // The chart is configured to rotate by 90 degrees when the mouse moves from
            // left to right, which is the plot region width (360 pixels). Similarly, the
            // elevation changes by 90 degrees when the mouse moves from top to buttom,
            // which is the plot region height (270 pixels).
            m_rotationAngle += (m_lastMouseX - mouseX) * 90.0 / 360;
            m_elevationAngle += (mouseY - m_lastMouseY) * 90.0 / 270;
            m_chartViewer->updateViewPort(true, false);
        }

        // Keep track of the last mouse position
        m_lastMouseX = mouseX;
        m_lastMouseY = mouseY;
        m_isDragging = true;
    }
    else if (m_isDragging)
    {
        // mouse up means not dragging
        m_isDragging = false;
        m_chartViewer->updateViewPort(true, false);
    }*/
}

void ThreeDChartRotation::OnExitClick(wxCommandEvent &event)
{
    wxUnusedVar(event);
    Close(true);
}

// wxEVT_CLOSE_WINDOW event handler for ID_CROSSHAIR
void ThreeDChartRotation::OnCloseWindow(wxCloseEvent &event)
{
    event.Skip();
}

void ThreeDChartRotation::OnButton1(wxCommandEvent &event)
{

    wxString str1;
    int temp;

    // 更新全局的antinfo
    int sel;
    sel = pBox->GetSelection();
    if (sel < ArrayInfo.size())
    {
        AntInfo = ArrayInfo[sel];
    }

    str1 = pTextTHNums->GetLineText(0);
    sscanf_s(str1.c_str(), "%d", &th_count);

    str1 = pTextPhiNums->GetLineText(0);
    sscanf_s(str1.c_str(), "%d", &ph_count);
    for (int i = 0; i < BEAM_ID_NUMBER; i++)
    {
        str1 = pTextTH[i]->GetLineText(0);
        sscanf_s(str1.c_str(), "%lf", &th0[i]);

        str1 = pTextPhi[i]->GetLineText(0);
        sscanf_s(str1.c_str(), "%lf", &ph0[i]);

        str1 = pTextCoefI[i]->GetLineText(0);
        sscanf_s(str1.c_str(), "%x", &temp);
        if (temp >= 0x800)
            temp = temp - 4096;

        CoefI[i] = temp / 2048.0;

        str1 = pTextCoefQ[i]->GetLineText(0);
        sscanf_s(str1.c_str(), "%x", &temp);
        if (temp >= 0x800)
            temp = temp - 4096;

        CoefQ[i] = temp / 2048.0;

        th0[i] = th0[i] / 180 * pi;
        ph0[i] = ph0[i] / 180 * pi;
    }
    str1 = pTextFreq->GetLineText(0);
    sscanf_s(str1.c_str(), "%lf", &f);
    f = f * 1000000; // To GHz

    str1 = pTextPhiView0->GetLineText(0);
    sscanf_s(str1.c_str(), "%lf", &ph0_view);

    if (phase != nullptr)
    {
        delete phase;
        phase = nullptr;
    }
    phase = new double[AntInfo.size()];

    mode_sel = pRadio->GetSelection();

    mglQT gr(smgl_surf3, "ant direction");

    // 将phase计算得到的相位值更新到表格中。
    // pGrid->SetColLabelValue(4,_("计算后的相位-弧度"));
    // pGrid->SetColLabelValue(5,_("计算后的相位-角度"));
    // pGrid->SetColLabelValue(6,_("计算后的相位-6bit定点化"));
    for (int i = 0; i < AntInfo.size(); i++)
    {
        pGrid->SetCellValue(i, 4, wxString::Format("%.3lf", phase[i]));
        pGrid->SetCellValue(i, 5, wxString::Format("%.3lf", phase[i] / pi * 180));

        double phase_adj;
        int phase_bin;
        if (phase[i] < 0)
            phase_adj = phase[i] + 2 * pi;
        else
            phase_adj = phase[i];

        phase_bin = ((int)(phase_adj / 2 / pi * 64 + 0.5)) % 64;
        pGrid->SetCellValue(i, 6, wxString::Format("%d", phase_bin));
    }
    pGrid->AutoSize();
    // Fit();
    // Refresh();
    Layout();
    pGrid->FitInside();
    gr.Run();
}

void ThreeDChartRotation::OnButtonLoad(wxCommandEvent &event)
{
    wxFileDialog
        openFileDialog(this, _("Open file"), "", "",
                       "all files (*.*)|*.*", wxFD_OPEN | wxFD_FILE_MUST_EXIST);

    if (openFileDialog.ShowModal() == wxID_CANCEL)
        return; // the user changed idea...

    // proceed loading the file chosen by the user;
    // this can be done with e.g. wxWidgets input streams:
    /*
    wxFileInputStream input_stream(openFileDialog.GetPath());
    if (!input_stream.IsOk())
    {
        wxLogError("Cannot open file '%s'.", openFileDialog.GetPath());
        return;
    }
    */
    wxString strFileName = openFileDialog.GetPath();
    wxString str;
    wxTextFile file;
    AntInfo.clear();
    file.Open(strFileName);
    STRUCTANTINFO sAntInfo;
    int i = 0;
    for (str = file.GetFirstLine(); !file.Eof(); str = file.GetNextLine())
    {
        // do something with the current line in str
        // 序号，x，y，z坐标。
        sscanf_s(str, "%d %lf %lf %lf", &sAntInfo.index, &sAntInfo.x, &sAntInfo.y, &sAntInfo.z);
        // mm转成米
        sAntInfo.x = sAntInfo.x / 1000;
        sAntInfo.y = sAntInfo.y / 1000;
        sAntInfo.z = sAntInfo.z / 1000;

        printf("ant[%d]=%lf,%lf,%lf\n", sAntInfo.index, sAntInfo.x, sAntInfo.y, sAntInfo.z);

        AntInfo[i++] = sAntInfo;
    }

    // do something with the last line in str
    sscanf_s(str, "%d %lf %lf %lf", &sAntInfo.index, &sAntInfo.x, &sAntInfo.y, &sAntInfo.z);
    AntInfo[sAntInfo.index] = sAntInfo;

    ArrayInfo.push_back(AntInfo);
    file.Close();
    // 将文件添加到列表控件，并且更新GRID
    UpdateGrid();

    str = openFileDialog.GetFilename();

    pBox->Insert(str, pBox->GetCount());

    pBox->SetSelection(pBox->GetCount() - 1);
}

void ThreeDChartRotation::OnMultiMode(wxCommandEvent &event)
{
    MultiMode = !MultiMode;
    gMultiMod = MultiMode;
    // Freeze();
    for (int i = 1; i < BEAM_ID_NUMBER; i++)
    {
        pSizer[i]->Show(MultiMode);
    }
    Fit();
    Layout();
    pGrid->FitInside();
    // Thaw();
    // Refresh();
}

void ThreeDChartRotation::OnClearCoef(wxCommandEvent &event)
{
    pTextCoefI[0]->Clear();
    (*pTextCoefI[0]) << "7ff";
    pTextCoefQ[0]->Clear();
    (*pTextCoefQ[0]) << "0";
    for (int i = 1; i < BEAM_ID_NUMBER; i++)
    {
        pTextCoefI[i]->Clear();
        (*pTextCoefI[i]) << "0";
        pTextCoefQ[i]->Clear();
        (*pTextCoefQ[i]) << "0";
    }
}

void ThreeDChartRotation::OnPhiToggle(wxCommandEvent &event)
{
    PhiRange = !PhiRange;
    gPhiRange = PhiRange;
    if (PhiRange == true)
        pToggleButton->SetLabel(wxT("PHI扫描范围360度"));
    else
        pToggleButton->SetLabel(wxT("PHI扫描范围180度"));
}

void ThreeDChartRotation::UpdateGrid()
{

    // 将所有的数据在pGrid中显示 天线编号，x坐标，y坐标，z坐标，计算相位弧度，计算相位角度
    pGrid->DeleteRows(0, pGrid->GetNumberRows());
    pGrid->DeleteCols(0, pGrid->GetNumberCols());

    pGrid->AppendRows(AntInfo.size());
    pGrid->AppendCols(7);
    pGrid->SetColLabelValue(0, wxT("天线编号"));
    pGrid->SetColLabelValue(1, wxT("x坐标"));
    pGrid->SetColLabelValue(2, wxT("y坐标"));
    pGrid->SetColLabelValue(3, wxT("z坐标"));
    pGrid->SetColLabelValue(4, wxT("计算后的相位-弧度"));
    pGrid->SetColLabelValue(5, wxT("计算后的相位-角度"));
    pGrid->SetColLabelValue(6, wxT("计算后的相位-6bit定点化"));

    for (int i = 0; i < AntInfo.size(); i++)
    {
        pGrid->SetCellValue(i, 0, wxString::Format("%d", AntInfo[i].index));
        pGrid->SetCellValue(i, 1, wxString::Format("%lf", AntInfo[i].x));
        pGrid->SetCellValue(i, 2, wxString::Format("%lf", AntInfo[i].y));
        pGrid->SetCellValue(i, 3, wxString::Format("%lf", AntInfo[i].z));
    }
    pGrid->AutoSize();
    // Fit();
    // Refresh();
    Layout();
    pGrid->FitInside();
}
