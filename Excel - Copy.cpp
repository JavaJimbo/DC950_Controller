// Excel.cpp: Implementierung der Klasse CExcel.
//
//////////////////////////////////////////////////////////////////////


#include "stdafx.h"
#include "Excel.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

/*
Constructor of your new Excel-sheet.
You my give a certain string as filetemplate. All
temporary created fiels will begin with this string
default ist XLS

*/
CExcel::CExcel(CString aTemp)
{
	m_FileTemplate=aTemp;

}

CExcel::~CExcel()
{

}

/*
After Styles and Columns have been defined, Open() creates the Excel-file
If you specify a name for your Excel-sheet, output will be formatted in XML-style
If you do not specify a name, a SCV-file will be created with no formatting at all
Excel wants to know the number of rows right at the beginning of the file, so you
should enter a number for the rows. When creating a CSV-file, the number of
rows is not necessary

*/
void CExcel::Open(int aLineCount, CString aMappe)
{
	char temppath[256];
	char TempFile[1024];
	int i;

	XML=(aMappe!="");

	GetTempPath(sizeof(temppath),temppath);
	srand( (unsigned)time( NULL ) );
	GetTempFileName(temppath,m_FileTemplate,rand(),TempFile);
	FName=TempFile;
	FName=FName.Left(FName.GetLength()-4);
	if(XML)
		FName=FName+".XML";
	else
		FName=FName+".CSV";
	handle=fopen(FName,"wt");
	if(XML)
	{
		fprintf(handle,"<?xml version=\"1.0\"?>\n");
		fprintf(handle,"<?mso-application progid=\"Excel.Sheet\"?>\n");
		fprintf(handle,"<Workbook xmlns=\"urn:schemas-microsoft-com:office:spreadsheet\"\n");
		fprintf(handle," xmlns:o=\"urn:schemas-microsoft-com:office:office\"\n");
		fprintf(handle," xmlns:x=\"urn:schemas-microsoft-com:office:excel\"\n");
		fprintf(handle," xmlns:ss=\"urn:schemas-microsoft-com:office:spreadsheet\">\n");

		fprintf(handle," <Styles>\n");
		fprintf(handle,"  <Style ss:ID=\"Default\" ss:Name=\"Normal\">\n");
		fprintf(handle,"   <Alignment ss:Vertical=\"Bottom\"/>\n");
		fprintf(handle,"  </Style>\n");
		
		for(i=0;i<Styles.GetSize();i++)
		{
			SetStyle(i);
		}
		fprintf(handle," </Styles>\n");
		fprintf(handle," <Worksheet ss:Name=\"%s\">\n",MakeXML(aMappe));
		fprintf(handle,"  <Table ss:ExpandedColumnCount=\"%i\" ss:ExpandedRowCount=\"%i\">\n",Columns.GetSize(),aLineCount);
		for(i=0;i<Columns.GetSize();i++)
		{
			if(Columns.GetAt(i)!="")
				fprintf(handle,"   <Column ss:Width=\"%s\"/>\n",Columns.GetAt(i));
			else
				fprintf(handle,"   <Column ss:AutoFitWidth=\"1\"/>\n");
		}

	}
}

/*
You created Excel-sheet will be displayed using Excel
*/
void CExcel::Run()
{
	ShellExecute(NULL,"open",FName,"","",SW_SHOW);
}

/*
Close() finishes creation of temporary Excel-Sheet, but does nothing else
Use Run() to start Excel
*/
void CExcel::Close()
{
	if(XML)
	{
		fprintf(handle,"  </Table>\n");
		fprintf(handle," </Worksheet>\n");
		fprintf(handle,"</Workbook>\n");
	}
	fclose(handle);
}

/*
Use Delete to delete your Excel-Sheet.

  WARNING:
XML-Files will not be deleted, otherwise, Excel will not be able to open the file
Use DeleteAll() at the end of your program instead

*/
void CExcel::Delete()
{
	if(!XML)
		DeleteFile(FName);
}

void CExcel::SetCell(CString aData, int aStyle)
{
	if(XML)
	{
		if(aStyle>-1)
			fprintf(handle,"    <Cell ss:StyleID=\"s%i\"><Data ss:Type=\"String\">%s</Data></Cell>\n",aStyle+21,MakeXML(aData));
		else
			fprintf(handle,"    <Cell><Data ss:Type=\"String\">%s</Data></Cell>\n",MakeXML(aData));
	}
	else
	{
		fprintf(handle,"\"%s\";",MakeXML(aData));
	}
}

/*
Data is organized in rows. So before adding data, you have to define a row.
The row may be formatted with a previously defined style and a certain height
*/
void CExcel::NewRow(int aStyle, float aHeight)
{
	if(XML)
	{
		fprintf(handle,"   <Row");
		if(aStyle>=0)
			fprintf(handle," ss:StyleID=\"s%i\"",aStyle+21);
		if(aHeight!=0)
			fprintf(handle," ss:Height=\"%.2f\"",aHeight);
		fprintf(handle,">\n");
	}
}

/*
EndRow() is used to indicate that a row of data cells is finished
*/
void CExcel::EndRow()
{
	if(XML)
		fprintf(handle,"   </Row>\n");
	else
		fprintf(handle,"\n");
}

/*
	SetStyle() is used when creating an XML-file to generate XML-statements
	for the required style
*/
void CExcel::SetStyle(int aStyle)
{
	CString Alignment;

	fprintf(handle,"  <Style ss:ID=\"s%i\">\n",aStyle+21);
	fprintf(handle,"   <Alignment");
	Alignment="";
	if((Styles.GetAt(aStyle)&XLS_HCENTER)==XLS_HCENTER)
		Alignment="Center";
	if((Styles.GetAt(aStyle)&XLS_HLEFT)==XLS_HLEFT)
		Alignment="Left";
	if((Styles.GetAt(aStyle)&XLS_HRIGHT)==XLS_HRIGHT)
		Alignment="Right";

	if(Alignment!="")
		fprintf(handle," ss:Horizontal=\"%s\"",Alignment);

	Alignment="";
	if((Styles.GetAt(aStyle)&XLS_VCENTER)==XLS_VCENTER)
		Alignment="Center";
	if((Styles.GetAt(aStyle)&XLS_VTOP)==XLS_VTOP)
		Alignment="Top";
	if((Styles.GetAt(aStyle)&XLS_VBOTTOM)==XLS_VBOTTOM)
		Alignment="Bottom";

	if(Alignment!="")
		fprintf(handle," ss:Vertical=\"%s\"",Alignment);
	
	if((Styles.GetAt(aStyle)&XLS_WRAP)==XLS_WRAP)
		fprintf(handle," ss:WrapText=\"1\"");
	fprintf(handle,"/>\n");

	if((Styles.GetAt(aStyle)&XLS_DATE)==XLS_DATE)
		fprintf(handle,"  <NumberFormat ss:Format=\"Short Date\"/>");

	if((Styles.GetAt(aStyle)&XLS_TIME)==XLS_TIME)
		fprintf(handle,"  <NumberFormat ss:Format=\"Short Time\"/>");

	if((Styles.GetAt(aStyle)&XLS_CURRENCY)==XLS_CURRENCY)
		fprintf(handle,"  <NumberFormat ss:Format=\"Currency\"/>");


	if((Styles.GetAt(aStyle)&XLS_BORDER)==XLS_BORDER)
	{
		fprintf(handle,"   <Borders>\n");
		fprintf(handle,"    <Border ss:Position=\"Bottom\" ss:LineStyle=\"Continuous\" ss:Weight=\"1\"/>\n");
		fprintf(handle,"    <Border ss:Position=\"Left\" ss:LineStyle=\"Continuous\" ss:Weight=\"1\"/>\n");
		fprintf(handle,"    <Border ss:Position=\"Right\" ss:LineStyle=\"Continuous\" ss:Weight=\"1\"/>\n");
		fprintf(handle,"    <Border ss:Position=\"Top\" ss:LineStyle=\"Continuous\" ss:Weight=\"1\"/>\n");
		fprintf(handle,"   </Borders>\n");
	}
	fprintf(handle,"   <Font x:Family=\"Swiss\"");
	Alignment=Sizes.GetAt(aStyle);
	if(Alignment!="")
		fprintf(handle," ss:Size=\"%s\"",Alignment);
	if((Styles.GetAt(aStyle)&XLS_BOLD)==XLS_BOLD)
		fprintf(handle," ss:Bold=\"1\"");
	if((Styles.GetAt(aStyle)&XLS_LINE)==XLS_LINE)
		fprintf(handle," ss:Underline=\"Single\"");
	if((Styles.GetAt(aStyle)&XLS_ITALICS)==XLS_ITALICS)
		fprintf(handle," ss:Italic=\"1\"");

	fprintf(handle,"/>\n");
	fprintf(handle,"  </Style>\n");
}

/*
Used to transfer a string to XML-Format or to delete \n\r from a line of text
You may add additional character replacements here
*/

CString CExcel::MakeXML(CString aStr)
{
	if(XML)
	{
		aStr.Replace("ö","&ouml;");
		aStr.Replace("ä","&auml;");
		aStr.Replace("ü","&uuml;");
		aStr.Replace("Ö","&Ouml;");
		aStr.Replace("Ä","&Auml;");
		aStr.Replace("Ü","&Uuml;");
		aStr.Replace("ß","&szlig;");
	}
	else
	{
		aStr.Replace('\n',' ');
		aStr.Replace('\r',' ');
	}
	return(aStr);
}

/*
AddStyle is used to define the style of every cell of data
whenever you want to combine different styles (Bold and underlined) 
Excel requires a new style to be defined. 
Just add the styles, you want to have and give an optional textsize
The return-value is a unique identifier for the style you created
Possible styles are defined in the header-file

*/
int CExcel::AddStyle(WORD aBit, CString aSize)
{
	int styleid;

	styleid=Styles.Add(aBit);
	Sizes.SetAtGrow(styleid,aSize);
	return(styleid);
}

/*
AddColumn is used to add a new column to your Excel-sheet
Before you can fill in any values, you have to define all your columns
You may specify a new width for your column. Otherwise Excel-default is used
*/
void CExcel::AddColumn(double awidth)
{
	CString anew;
	if(awidth!=0.0)
		anew.Format("%.2f",awidth);
	else
		anew="";
	Columns.Add(anew);
}

/*
Use DeleteAll() to delete all temporary files you created at the end of your program
*/
void CExcel::DeleteAll()
{
	char temppath[1024];
	WIN32_FIND_DATA FindFileData;
	HANDLE hFind;
	BOOL found=FALSE;

	GetTempPath(sizeof(temppath),temppath);
	FName.Format("%s\\%s*",temppath,m_FileTemplate);

	hFind = FindFirstFile(FName, &FindFileData);
	if(hFind)
		found=TRUE;
	while(found)
	{
		FName.Format("%s\\%s",temppath,FindFileData.cFileName);
		DeleteFile(FName);
		found=FindNextFile(hFind,&FindFileData);
	}
    FindClose(hFind);

}

/*
SetCell sets the value and the formatting of a cell of data.
CExcel can display the following values:

  String
  Date
  Time
  Currency
  Integer Number
  Real Number

  To display date, time and currency you have to define a style
*/
void CExcel::SetCell(CTime aData, int aStyle)
{
	if(XML)
	{
		if(aStyle>-1)
			fprintf(handle,"    <Cell ss:StyleID=\"s%i\"><Data ss:Type=\"DateTime\">%s</Data></Cell>\n",aStyle+21,aData.Format("%Y-%m-%dT%H:%M:%S"));
		else
			fprintf(handle,"    <Cell><Data ss:Type=\"DateTime\">%s</Data></Cell>\n",aData.Format("%Y-%m-%dT%H:%M:%S"));
	}
	else
	{
		fprintf(handle,"\"%s\";",aData.Format("%d.%m.%Y"));
	}

}

void CExcel::SetCell(int aData, int aStyle)
{
	if(XML)
	{
		if(aStyle>-1)
			fprintf(handle,"    <Cell ss:StyleID=\"s%i\"><Data ss:Type=\"Number\">%i</Data></Cell>\n",aStyle+21,aData);
		else
			fprintf(handle,"    <Cell><Data ss:Type=\"Number\">%i</Data></Cell>\n",aData);
	}
	else
	{
		fprintf(handle,"\"%i\";",aData);
	}

}

void CExcel::SetCell(float aData, int aStyle)
{
	if(XML)
	{
		if(aStyle>-1)
			fprintf(handle,"    <Cell ss:StyleID=\"s%f\"><Data ss:Type=\"Number\">%i</Data></Cell>\n",aStyle+21,aData);
		else
			fprintf(handle,"    <Cell><Data ss:Type=\"Number\">%f</Data></Cell>\n",aData);
	}
	else
	{
		fprintf(handle,"\"%f\";",aData);
	}

}
