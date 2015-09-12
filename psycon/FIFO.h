// FIFO.h: interface for the CDoubleFIFO class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FIFO_H__DD87701E_B669_406A_8342_A5D63E2F4076__INCLUDED_)
#define AFX_FIFO_H__DD87701E_B669_406A_8342_A5D63E2F4076__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define MAX_FIFO 16

class CDoubleFIFO  
{
public:
	inline void SetLength(int newLen) { length = min (newLen, MAX_FIFO); }
	inline double get() {return pt[0];}
	void set(double newval);
	CDoubleFIFO(int len=1, double initVal=0.);
	virtual ~CDoubleFIFO();
private:
	int length;
	double pt[MAX_FIFO];
};

class CShortFIFO
{
public:
	inline void SetLength(int newLen) { length = min (newLen, MAX_FIFO); }
	inline short get() {return pt[0];}
	void set(short newval);
	CShortFIFO(int len=1, short initVal=-1);
	virtual ~CShortFIFO();
private:
	int length;
	short pt[MAX_FIFO];
};


#endif // !defined(AFX_FIFO_H__DD87701E_B669_406A_8342_A5D63E2F4076__INCLUDED_)
