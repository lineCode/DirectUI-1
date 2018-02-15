

#include "StdAfx.h"
#include "UILabelEx2.h"

#include <atlconv.h>



namespace DirectUI
{
	CLabelEx2::CLabelEx2()
	{
	}


	CLabelEx2::~CLabelEx2()
	{
	}


	Color CLabelEx2::_MakeRGB(int a, Color cl)
	{
		return Color(a, cl.GetR(), cl.GetG(), cl.GetB());
	}


	Color CLabelEx2::_MakeRGB(int r, int g, int b)
	{
		return Color(255, r, g, b);
	}


	void CLabelEx2::SetMultiline()
	{
		DWORD dwStyle = GetTextStyle();
		dwStyle |= DT_WORDBREAK;
		SetTextStyle(dwStyle);
	}


	void CLabelEx2::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
	{
		if (_tcscmp(pstrName, _T("multiline")) == 0)
		{
			if (_tcscmp(pstrValue, _T("true")) == 0)
				SetMultiline();
		}

		__super::SetAttribute(pstrName, pstrValue);
	}


	void CLabelEx2::PaintText(HDC hDC)
	{
		UINT uStyle = GetTextStyle();  
		if( m_dwTextColor == 0 ) m_dwTextColor = m_pManager->GetDefaultFontColor();  
		if( m_dwDisabledTextColor == 0 ) m_dwDisabledTextColor = m_pManager->GetDefaultDisabledColor();  

		RECT rc = m_rcItem;  
		rc.left += m_rcTextPadding.left;  
		rc.right -= m_rcTextPadding.right;  
		rc.top += m_rcTextPadding.top;  
		rc.bottom -= m_rcTextPadding.bottom;  

		if(!GetEnabledEffect())  
		{  
			if( m_sText.IsEmpty() ) return;  
			int nLinks = 0;  
			if( IsEnabled() ) {  
				if( m_bShowHtml )  
					CRenderEngine::DrawHtmlText(hDC, m_pManager, rc, m_sText, m_dwTextColor,
					NULL, NULL, nLinks, DT_SINGLELINE | m_uTextStyle);  
				else  
					CRenderEngine::DrawText(hDC, m_pManager, rc, m_sText, m_dwTextColor,
					m_iFont, /* DT_SINGLELINE | */m_uTextStyle); ///< @todo  
			}  
			else {  
				if( m_bShowHtml )  
					CRenderEngine::DrawHtmlText(hDC, m_pManager, rc, m_sText, m_dwDisabledTextColor,
					NULL, NULL, nLinks, DT_SINGLELINE | m_uTextStyle);  
				else  
					CRenderEngine::DrawText(hDC, m_pManager, rc, m_sText, m_dwDisabledTextColor,
					m_iFont, /* DT_SINGLELINE | */m_uTextStyle); ///< @todo  
			}  
		}  
		else  
		{  
			Font    nFont(hDC,m_pManager->GetFont(GetFont()));  

			Graphics nGraphics(hDC);  
			nGraphics.SetTextRenderingHint(m_TextRenderingHintAntiAlias);  

			StringFormat format;  
			format.SetAlignment((StringAlignment)m_hAlign);  
			format.SetLineAlignment((StringAlignment)m_vAlign);  

			RectF nRc((float)rc.left,(float)rc.top,(float)rc.right-rc.left,(float)rc.bottom-rc.top);  
			RectF nShadowRc = nRc;  
			nShadowRc.X += m_ShadowOffset.X;  
			nShadowRc.Y += m_ShadowOffset.Y;  

			int nGradientLength = GetGradientLength();  

			if(nGradientLength == 0)  
				nGradientLength = (rc.bottom-rc.top);  

			LinearGradientBrush nLineGrBrushA(Point(GetGradientAngle(), 0),Point(0,nGradientLength),_MakeRGB(GetTransShadow(),GetTextShadowColorA()),_MakeRGB(GetTransShadow1(),GetTextShadowColorB() == -1?GetTextShadowColorA():GetTextShadowColorB()));  
			LinearGradientBrush nLineGrBrushB(Point(GetGradientAngle(), 0),Point(0,nGradientLength),_MakeRGB(GetTransText(),GetTextColor()),_MakeRGB(GetTransText1(),GetTextColor1() == -1?GetTextColor():GetTextColor1()));  

			if(GetEnabledStroke() && GetStrokeColor() > 0)  
			{  
				LinearGradientBrush nLineGrBrushStroke(Point(GetGradientAngle(),0),Point(0,rc.bottom-rc.top+2),_MakeRGB(GetTransStroke(),GetStrokeColor()),_MakeRGB(GetTransStroke(),GetStrokeColor()));  

#ifdef _UNICODE  
				nRc.Offset(-1,0);  
				nGraphics.DrawString(m_TextValue,m_TextValue.GetLength(),&nFont,nRc,&format,&nLineGrBrushStroke);  
				nRc.Offset(2,0);  
				nGraphics.DrawString(m_TextValue,m_TextValue.GetLength(),&nFont,nRc,&format,&nLineGrBrushStroke);  
				nRc.Offset(-1,-1);  
				nGraphics.DrawString(m_TextValue,m_TextValue.GetLength(),&nFont,nRc,&format,&nLineGrBrushStroke);  
				nRc.Offset(0,2);  
				nGraphics.DrawString(m_TextValue,m_TextValue.GetLength(),&nFont,nRc,&format,&nLineGrBrushStroke);  
				nRc.Offset(0,-1);  
#else  
				USES_CONVERSION;  
				wstring mTextValue = A2W(m_TextValue.GetData());  

				nRc.Offset(-1,0);  
				nGraphics.DrawString(mTextValue.c_str(),mTextValue.length(),&nFont,nRc,&format,&nLineGrBrushStroke);  
				nRc.Offset(2,0);  
				nGraphics.DrawString(mTextValue.c_str(),mTextValue.length(),&nFont,nRc,&format,&nLineGrBrushStroke);  
				nRc.Offset(-1,-1);  
				nGraphics.DrawString(mTextValue.c_str(),mTextValue.length(),&nFont,nRc,&format,&nLineGrBrushStroke);  
				nRc.Offset(0,2);  
				nGraphics.DrawString(mTextValue.c_str(),mTextValue.length(),&nFont,nRc,&format,&nLineGrBrushStroke);  
				nRc.Offset(0,-1);  
#endif  

			}  
#ifdef _UNICODE  
			if(GetEnabledShadow() && (GetTextShadowColorA() > 0 || GetTextShadowColorB() > 0))  
				nGraphics.DrawString(m_TextValue,m_TextValue.GetLength(),&nFont,nShadowRc,&format,&nLineGrBrushA);  

			nGraphics.DrawString(m_TextValue,m_TextValue.GetLength(),&nFont,nRc,&format,&nLineGrBrushB);  
#else  
			USES_CONVERSION;  
			wstring mTextValue = A2W(m_TextValue.GetData());  

			if(GetEnabledShadow() && (GetTextShadowColorA() > 0 || GetTextShadowColorB() > 0))  
				nGraphics.DrawString(mTextValue.c_str(),mTextValue.length(),&nFont,nShadowRc,&format,&nLineGrBrushA);  

			nGraphics.DrawString(mTextValue.c_str(),mTextValue.length(),&nFont,nRc,&format,&nLineGrBrushB);  
#endif  

		}
	}


	void CLabelEx2::SetPos(RECT rc)
	{
		__super::SetPos(rc);
	}
}

