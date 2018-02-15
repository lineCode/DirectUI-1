
#ifndef __UILABEL_EX2_H__
#define __UILABEL_EX2_H__

#pragma once


namespace DirectUI
{
	class DirectUI_API CLabelEx2 : public CLabelUI
	{
	public:
		CLabelEx2();
		virtual ~CLabelEx2();

		virtual void PaintText(HDC hDC);
		virtual void SetPos(RECT rc);
		virtual void SetMultiline();
		void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);

	private:
		Color _MakeRGB(int a, Color cl);
		Color _MakeRGB(int r, int g, int b);
	};
}



#endif //__UILABEL_EX2_H__