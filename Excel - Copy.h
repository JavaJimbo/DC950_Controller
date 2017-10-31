// Excel.h: Schnittstelle für die Klasse CExcel.
//
//////////////////////////////////////////////////////////////////////

/*
Create XML-Files for use with Excel from within C++ Application

  Usage:

  Declare variable of type CExcel
  define all used styles in your excel-sheet
  add columns (with optional width of column)
  open the file
  add row (with optional style and height)
  set cell value with optional style
  end row
  close file
  run the file to open excel
  delete the file after usage (or delete all files at end of program)
  */


#if !defined(AFX_EXCEL_H__146DB75A_DA71_48C6_A5ED_50ED23F47648__INCLUDED_)
#define AFX_EXCEL_H__146DB75A_DA71_48C6_A5ED_50ED23F47648__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#define XLS_NOSTYLE -1
#define XLS_HCENTER 1
#define XLS_HLEFT 2
#define XLS_HRIGHT 4
#define XLS_VTOP 8
#define XLS_VCENTER 16
#define XLS_VBOTTOM 32
#define XLS_WRAP 64
#define XLS_BORDER 128
#define XLS_BOLD 256
#define XLS_LINE 512
#define XLS_ITALICS 1024
#define XLS_DATE 2048
#define XLS_TIME 4096
#define XLS_CURRENCY 8192


class CExcel  
{
public:
	CExcel(CString aTemp="XLS");
	virtual ~CExcel();
	void SetCell(float aData, int aStyle=XLS_NOSTYLE);
	void SetCell(int aData, int aStyle=XLS_NOSTYLE);
	void SetCell(CTime aData, int aStyle=XLS_NOSTYLE);
	void SetCell(CString aData, int aStyle=XLS_NOSTYLE);
	void DeleteAll();
	void AddColumn(double awidth=0.0);
	void EndRow();
	void NewRow(int aStyle=XLS_NOSTYLE, float aHeight=0.0);
	void Delete();
	void Run();
	void Close();
	void Open(int aLineCount=0, CString aMappe="");
	int AddStyle(WORD aBit, CString aSize="");
	CString MakeXML(CString aStr);
	CString m_FileTemplate;
private:
	void SetStyle(int aStyle);
	CString FName;
	FILE *handle;
	BOOL XML;
	CStringArray Sizes;
	CStringArray Columns;
	CWordArray Styles;

};

// #endif // !defined(AFX_EXCEL_H__146DB75A_DA71_48C6_A5ED_50ED23F47648__INCLUDED_)
