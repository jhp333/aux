// Adaptive.h: interface for the CAdaptive class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ADAPTIVE_H__3475FD30_AB60_4A9C_801E_9DCE8B4895B9__INCLUDED_)
#define AFX_ADAPTIVE_H__3475FD30_AB60_4A9C_801E_9DCE8B4895B9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define ANY_NUMBER_INT		999

class CAdaptive  
{
public:
	bool descending;
	bool warning;
	int dualSeries; // This is referenced only by main adap in AdaptiveDlg
	int reversalUpdate (double variable);
	void InitAdaptive(int *corrHist, double *thrCand);
	int Proceed(int corr, double *variable);
	double	stepsize[5];
	bool    factor; //if this is set, variable is adjusted by the factor, not by the step difference
	int		reversal[5];
	int		down;
	int		up;
	int		finalRev4Avg;
	double	initialPt;
	int		m_limit;
	CAdaptive();
	virtual ~CAdaptive();

private:  // not to be modified by user of the class
	int nCandidates;
	int maxReversalStage;
	double * thresholdCandidate;
	int * correctHistory;
	int fineStreak;
	int rev;
	int revStage;
	int trial;
	int	hitStreak; 
	int	missStreak; 
	int GetFirstNonZeroItems(int *array, int length_limit);
};

#endif // !defined(AFX_ADAPTIVE_H__3475FD30_AB60_4A9C_801E_9DCE8B4895B9__INCLUDED_)
