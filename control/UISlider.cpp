#include "StdAfx.h"
#include "UISlider.h"

namespace DirectUI
{
	CSliderUI::CSliderUI() : m_uButtonState(0), m_nStep(1),m_bDoubleClicked(false),m_bOnlyInThumb(false)
	{
		m_uTextStyle = DT_SINGLELINE | DT_CENTER;
		m_szThumb.cx = m_szThumb.cy = 10;
	}

	LPCTSTR CSliderUI::GetClass() const
	{
		return _T("SliderUI");
	}

	UINT CSliderUI::GetControlFlags() const
	{
		if (IsEnabled()) return UIFLAG_SETCURSOR;
		else return 0;
	}

	LPVOID CSliderUI::GetInterface(LPCTSTR pstrName)
	{
		if (_tcscmp(pstrName, DUI_CTR_SLIDER) == 0) return static_cast<CSliderUI*>(this);
		return CProgressUI::GetInterface(pstrName);
	}

	void CSliderUI::SetEnabled(bool bEnable)
	{
		CControlUI::SetEnabled(bEnable);
		if (!IsEnabled())
			m_uButtonState = 0;
	}

	int CSliderUI::GetChangeStep()
	{
		return m_nStep;
	}

	void CSliderUI::SetChangeStep(int step)
	{
		m_nStep = step;
	}

	void CSliderUI::SetThumbSize(SIZE szXY)
	{
		m_szThumb = szXY;
	}

	RECT CSliderUI::GetThumbRect() const
	{
		if (m_bHorizontal)
		{
			int left = m_rcItem.left + (m_rcItem.right - m_rcItem.left - m_szThumb.cx) * (m_nValue - m_nMin) / (m_nMax - m_nMin);
			int top = (m_rcItem.bottom + m_rcItem.top - m_szThumb.cy) / 2;
			return CRect(left, top, left + m_szThumb.cx, top + m_szThumb.cy); 
		}
		else 
		{
			int left = (m_rcItem.right + m_rcItem.left - m_szThumb.cx) / 2;
			int top = m_rcItem.bottom - m_szThumb.cy - (m_rcItem.bottom - m_rcItem.top - m_szThumb.cy) * (m_nValue - m_nMin) / (m_nMax - m_nMin);
			return CRect(left, top, left + m_szThumb.cx, top + m_szThumb.cy); 
		}
	}

	LPCTSTR CSliderUI::GetThumbImage() const
	{
		return m_sThumbImage;
	}

	void CSliderUI::SetThumbImage(LPCTSTR pStrImage)
	{
		m_sThumbImage = pStrImage;
		Invalidate();
	}

	LPCTSTR CSliderUI::GetThumbHotImage() const
	{
		return m_sThumbHotImage;
	}

	void CSliderUI::SetThumbHotImage(LPCTSTR pStrImage)
	{
		m_sThumbHotImage = pStrImage;
		Invalidate();
	}

	LPCTSTR CSliderUI::GetThumbPushedImage() const
	{
		return m_sThumbPushedImage;
	}

	void CSliderUI::SetThumbPushedImage(LPCTSTR pStrImage)
	{
		m_sThumbPushedImage = pStrImage;
		Invalidate();
	}

	void CSliderUI::DoEvent(TEventUI& event)
	{
		if (!IsMouseEnabled() && event.Type > UIEVENT__MOUSEBEGIN && event.Type < UIEVENT__MOUSEEND) 
		{
			if( m_pParent != NULL ) m_pParent->DoEvent(event);
			else CProgressUI::DoEvent(event);
			return;
		}

		if(event.Type == UIEVENT_BUTTONDOWN)
		{
			bool bShiftKey = (GetKeyState(VK_SHIFT)&0x8000);
			if (bShiftKey)
			{
				return;
			}
			if(IsEnabled()) 
			{
				RECT rcThumb = GetThumbRect();
				if (::PtInRect(&rcThumb, event.ptMouse)) 
				{
					m_uButtonState |= UISTATE_CAPTURED;
				}

				m_pManager->SendNotify(this, DUI_MSGTYPE_THUMB_BUTTONDOWN);
			}
			return;
		}

		if (event.Type == UIEVENT_DBLCLICK)
		{
			m_bDoubleClicked = true;
		}

		if (event.Type == UIEVENT_BUTTONUP)
		{
			bool bShiftKey = (GetKeyState(VK_SHIFT)&0x8000);
			if (bShiftKey)
			{
				if ((m_uButtonState & UISTATE_CAPTURED) != 0) 
				{
					m_uButtonState &= ~UISTATE_CAPTURED;
				}
				return;
			}
			if (!IsEnabled())
				return;

			if (m_bDoubleClicked)
			{
				m_bDoubleClicked = false;
				return;
			}

			int nValue = 0;;
			if ((m_uButtonState & UISTATE_CAPTURED) != 0) 
			{
				m_uButtonState &= ~UISTATE_CAPTURED;
			}
			else if(m_bOnlyInThumb)
			{
				return;
			}

			

			if (m_bHorizontal)
			{
				if (event.ptMouse.x >= m_rcItem.right - m_szThumb.cx / 2) nValue = m_nMax;
				else if (event.ptMouse.x <= m_rcItem.left + m_szThumb.cx / 2) nValue = m_nMin;
				else nValue = m_nMin + (m_nMax - m_nMin) * (event.ptMouse.x - m_rcItem.left - m_szThumb.cx / 2 ) / (m_rcItem.right - m_rcItem.left - m_szThumb.cx);
			}
			else 
			{
				if (event.ptMouse.y >= m_rcItem.bottom - m_szThumb.cy / 2) nValue = m_nMin;
				else if (event.ptMouse.y <= m_rcItem.top + m_szThumb.cy / 2) nValue = m_nMax;
				else nValue = m_nMin + (m_nMax - m_nMin) * (m_rcItem.bottom - event.ptMouse.y - m_szThumb.cy / 2 ) / (m_rcItem.bottom - m_rcItem.top - m_szThumb.cy);
			}

			if(m_nValue != nValue && nValue >= m_nMin && nValue <= m_nMax)
			{
				m_nValue = nValue;
				m_pManager->SendNotify(this, DUI_MSGTYPE_THUMB_BUTTONUP);
				m_pManager->SendNotify(this, DUI_MSGTYPE_VALUECHANGED);
				Invalidate();
			}
			else
			{
				m_pManager->SendNotify(this, DUI_MSGTYPE_THUMB_BUTTONUP);
			}

			return;
		}

		if (event.Type == UIEVENT_CONTEXTMENU)
		{
			return;
		}

		if (event.Type == UIEVENT_SCROLLWHEEL) 
		{
			if (!IsEnabled())
				return;
			switch (LOWORD(event.wParam))
			{
			case SB_LINEUP:
				SetValue(GetValue() + GetChangeStep());
				m_pManager->SendNotify(this, DUI_MSGTYPE_VALUECHANGED);
				return;
			case SB_LINEDOWN:
				SetValue(GetValue() - GetChangeStep());
				m_pManager->SendNotify(this, DUI_MSGTYPE_VALUECHANGED);
				return;
			}
		}

		if (event.Type == UIEVENT_MOUSEMOVE)
		{
			if (IsEnabled()) {
				if ((m_uButtonState & UISTATE_CAPTURED) != 0) 
				{
					if (m_bHorizontal) 
					{
						if (event.ptMouse.x >= m_rcItem.right - m_szThumb.cx / 2) 
							m_nValue = m_nMax;
						else if (event.ptMouse.x <= m_rcItem.left + m_szThumb.cx / 2) 
							m_nValue = m_nMin;
						else 
							m_nValue = m_nMin + (m_nMax - m_nMin) * (event.ptMouse.x - m_rcItem.left - m_szThumb.cx / 2 ) / (m_rcItem.right - m_rcItem.left - m_szThumb.cx);
					}
					else 
					{
						if (event.ptMouse.y >= m_rcItem.bottom - m_szThumb.cy / 2) 
							m_nValue = m_nMin;
						else if (event.ptMouse.y <= m_rcItem.top + m_szThumb.cy / 2) 
							m_nValue = m_nMax;
						else 
							m_nValue = m_nMin + (m_nMax - m_nMin) * (m_rcItem.bottom - event.ptMouse.y - m_szThumb.cy / 2 ) / (m_rcItem.bottom - m_rcItem.top - m_szThumb.cy);
					}
					m_pManager->SendNotify(this, DUI_MSGTYPE_VALUECHANGED);

					Invalidate();
				}

			}
			return;
		}

		if (event.Type == UIEVENT_SETCURSOR)
		{
			RECT rcThumb = GetThumbRect();
			if (IsEnabled() && ::PtInRect(&rcThumb, event.ptMouse))
			{
				::SetCursor(::LoadCursor(NULL, MAKEINTRESOURCE(IDC_HAND)));
				return;
			}
		}

		if (event.Type == UIEVENT_MOUSEENTER)
		{
			if (IsEnabled())
			{
				m_uButtonState |= UISTATE_HOT;
				Invalidate();
			}
			return;
		}

		if (event.Type == UIEVENT_MOUSELEAVE)
		{
			if (IsEnabled()) 
			{
				m_uButtonState &= ~UISTATE_HOT;
				Invalidate();
			}
			return;
		}

		CControlUI::DoEvent(event);
	}


	void CSliderUI::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
	{
		if (_tcscmp(pstrName, _T("thumbimage")) == 0) SetThumbImage(pstrValue);
		else if(_tcscmp(pstrName, _T("thumbhotimage")) == 0) SetThumbHotImage(pstrValue);
		else if(_tcscmp(pstrName, _T("thumbpushedimage")) == 0) SetThumbPushedImage(pstrValue);
		else if (_tcscmp(pstrName, _T("thumbinteract")) == 0) m_bOnlyInThumb = _tcscmp(pstrValue, _T("true")) == 0; //only interact with thumb
		else if(_tcscmp(pstrName, _T("thumbsize")) == 0)
		{
			SIZE szXY = {0};
			LPTSTR pstr = NULL;
			szXY.cx = _tcstol(pstrValue, &pstr, 10);  ASSERT(pstr);    
			szXY.cy = _tcstol(pstr + 1, &pstr, 10);    ASSERT(pstr); 
			SetThumbSize(szXY);
		}
		else if (_tcscmp(pstrName, _T("step")) == 0) SetChangeStep(_ttoi(pstrValue));
		else CProgressUI::SetAttribute(pstrName, pstrValue);
	}

	void CSliderUI::PaintStatusImage(HDC hDC)
	{
		if (m_nMax <= m_nMin) m_nMax = m_nMin + 1;
		if (m_nValue > m_nMax) m_nValue = m_nMax;
		if (m_nValue < m_nMin) m_nValue = m_nMin;

		RECT rc = {0};
		int nThumbValue=0;
		if (!m_sThumbImage.IsEmpty()) {
			if (m_bHorizontal)
				nThumbValue=m_szThumb.cx;
			else
				nThumbValue=m_szThumb.cy;
		}
		if (m_bHorizontal) 
		{
			rc.right = (m_nValue - m_nMin) * (m_rcItem.right - m_rcItem.left-nThumbValue) / (m_nMax - m_nMin)+nThumbValue/2;
			rc.bottom = m_rcItem.bottom - m_rcItem.top;
		}
		else
		{
			rc.top = (m_rcItem.bottom - m_rcItem.top-nThumbValue) * (m_nMax - m_nValue) / (m_nMax - m_nMin)+nThumbValue/2;
			rc.right = m_rcItem.right - m_rcItem.left;
			rc.bottom = m_rcItem.bottom - m_rcItem.top;
		}

		if (!m_sForeImage.IsEmpty()) 
		{
			m_sForeImageModify.Empty();
			if (m_bStretchForeImage)
				m_sForeImageModify.SmallFormat(_T("dest='%d,%d,%d,%d'"), rc.left, rc.top, rc.right, rc.bottom);
			else
				m_sForeImageModify.SmallFormat(_T("dest='%d,%d,%d,%d' source='%d,%d,%d,%d'")
				, rc.left, rc.top, rc.right, rc.bottom
				, rc.left, rc.top, rc.right, rc.bottom);

			if (!DrawImage(hDC, (LPCTSTR)m_sForeImage, (LPCTSTR)m_sForeImageModify)) 
				m_sForeImage.Empty();
		}

		RECT rcThumb = GetThumbRect();
		rcThumb.left -= m_rcItem.left;
		rcThumb.top -= m_rcItem.top;
		rcThumb.right -= m_rcItem.left;
		rcThumb.bottom -= m_rcItem.top;
		if ((m_uButtonState & UISTATE_CAPTURED) != 0) 
		{
			if (!m_sThumbPushedImage.IsEmpty())
			{
				m_sImageModify.Empty();
				m_sImageModify.SmallFormat(_T("dest='%d,%d,%d,%d'"), rcThumb.left, rcThumb.top, rcThumb.right, rcThumb.bottom);
				if (!DrawImage(hDC, (LPCTSTR)m_sThumbPushedImage, (LPCTSTR)m_sImageModify)) 
					m_sThumbPushedImage.Empty();
				else return;
			}
		}
		else if ((m_uButtonState & UISTATE_HOT) != 0)
		{
			if (!m_sThumbHotImage.IsEmpty()) 
			{
				m_sImageModify.Empty();
				m_sImageModify.SmallFormat(_T("dest='%d,%d,%d,%d'"), rcThumb.left, rcThumb.top, rcThumb.right, rcThumb.bottom);
				if (!DrawImage(hDC, (LPCTSTR)m_sThumbHotImage, (LPCTSTR)m_sImageModify)) 
					m_sThumbHotImage.Empty();
				else return;
			}
		}

		if (!m_sThumbImage.IsEmpty()) 
		{
			m_sImageModify.Empty();
			m_sImageModify.SmallFormat(_T("dest='%d,%d,%d,%d'"), rcThumb.left, rcThumb.top, rcThumb.right, rcThumb.bottom);
			if (!DrawImage(hDC, (LPCTSTR)m_sThumbImage, (LPCTSTR)m_sImageModify)) 
				m_sThumbImage.Empty();
			else return;
		}
	}
}
