#include "audfret.h"
#include <math.h>
#include "calbase.h"
#include <time.h>

#include "sigproc.h"

#include "audstr.h"

#define UNKNOWNVAR "Unknown variable or function: "

int countChar(const char *str, char const c, int *maxLength, int *minLength)
{
	// minLength and maxLength are the minimum and maximum length of the string broken down by 'c'
	int count(0);
	*maxLength = -1;
	*minLength = 99999;
	const char *ploc(str), *ploc2(str);
	
	while (ploc2!=NULL)
	{
		ploc2 = strchr(ploc, c);
		if (ploc2!=NULL)
		{
			*maxLength = max (*maxLength , (int)(ploc2-ploc));
			*minLength = min (*minLength , (int)(ploc2-ploc));
			ploc = ploc2+1;
		}
		else
		{
			*maxLength = max (*maxLength , (int)strlen(ploc));
			*minLength = min (*minLength , (int)strlen(ploc));
		}
		count++;
	}
	if (count==0) {*maxLength = *minLength = (int)strlen(str); }
	return count;
}

int CheckTagItem (string &strIn, string &estr)
{
	string buf[2];
	if (countDeliminators(strIn.c_str(), "=")!=2) 
	{estr = "(Each line should be : (tag_var) = (expression) )"; return 0;}
	str2array(buf, 2, strIn.c_str(), "="); 
	trim(buf[0],' ');
	if (buf[0].find_first_of (" `[]\\;',./~!@#$%^&*()_+{}|:""<>?")!=string::npos) 
	{ sformat(estr, 128, "Invalid variable name: %s", buf[0].c_str());return 0; }
	estr="";
	return 1;
}

int CountParenLayers (string strIn, char c, int order)
{  // check if the order-th c is inside of a parenthesis or not. 
	// returns the diff of ( and ). Returns zero if c is outside.
	
	// no checking parenthesis matching
	size_t id(0);
	string st(strIn);
	for (int i=0; i<order+1; i++)
	{
		id = st.find(c,id);
		if (id==string::npos) return -1;
		id++;
	}
	string sub = strIn.substr(0, id);
	int c1=countchar(sub.c_str(), '(');
	int c2=countchar(sub.c_str(), ')');
	return c1-c2;
}

int CEqSig::ParseLRHS(string &tagname, string &expression, string &operation)
{
	int res(1); // return 1 for logical expression, -1 for assignment (=), 0 for single expression (no assignment)
	short check[4];
	size_t id1(0), id2(0), id3(0), poseq(0);
	string _str(str), ins, out[2], str1, str2;
	out[0]=tagname; out[1]=expression;
	check[0] = (short)countstr(str, "==");
	check[1] = (short)countstr(str, "!=");
	check[2] = (short)countstr(str, "<=");
	check[3] = (short)countstr(str, ">=");
	if (sumarray(check,4)>1) throw "more than one logical operations"; 
	str2array(out, 2, str, "<>!=");
	if (sumarray(check,4)==1)
	{
		if (check[0]) operation = "==";
		else if (check[1]) operation = "!=";
		else if (check[2]) operation = "<=";
		else if (check[3]) operation = ">=";
	}
	else
	{
		check[0] = (short)countchar(str, '>');
		check[1] = (short)countchar(str, '<');
		check[2] = (short)countchar(str, '=');
		short temp = check[2];
		ins = str;
		for (int i=0; i<temp; i++)
		{
			id2 = ins.find("=", id1);
			id3 = ins.find("=", id2+1);
			if (CountParenLayers(ins, '=', i)) check[2]--; 
			else poseq = id2;
			id1 = id2+1; 
		}
		if (sumarray(check,3)>1) throw "more than one operators (< > =)"; 
		if (check[0]) operation = '>';
		else if (check[1]) operation = '<';
		else if (check[2]) 
		{
			operation = '=';
			res = -1;
			if (poseq>0) out[1] = _str.substr(poseq+1);
		}
		else
		{
			tagname="";
			expression = _str;
			operation="";
			return 0;
		}
	}
	tagname= out[0]; expression = out[1];
	return res;
}

void* colonednums(CSignals &sigout, double x1, double x2, double x3)
{
	CSignals out(-1); // double signal
	int nItems;
	nItems = max(1, (int)((x3-x1)/x2)+1);
	out.UpdateBuffer(nItems);
	for (int i=0; i<nItems; i++) out.buf[i] = x1 + (double)i*x2;
	sigout = out;
	return sigout.buf;
}

static void* colonednums(CSignals &sigout, double x1, double x2)
{
	return colonednums(sigout, x1, 1., x2);
}

int IsString(const char* str)
{
	string _str(str);
	trim(_str,' ');

	if (_str[0]=='\x22' && _str[_str.length()-1]=='\x22') return 1;
	else return 0;
}

int IsAllNumeric (string str)
{ // if str is all numeric returns -1
  // Otherwise, returns the first z-based index with nonnumeric char.
	trim(str,' ');
	//Get the first non-numeric char
	if (str.find_first_not_of("+-.1234567890")==string::npos ) return 1;
	else return 0;
}

void AddTag(int &ntags, CEqSig **tags, CEqSig *tag2add, int id2beInserted)
{
	CEqSig *newTagList = new CEqSig [++ntags];
	if (id2beInserted<0 || id2beInserted>ntags-1) id2beInserted = ntags-1;
	for (int i=0; i<id2beInserted; i++) 
		newTagList[i] = *tags[i];
	newTagList[id2beInserted] = *tag2add;
	for (int i=id2beInserted+1; i<ntags; i++) 
		newTagList[i] = (*tags)[i-1];
	delete[] *tags;
	*tags = newTagList;
}

EXP_CS CEqSig::CEqSig(void)
:maxCalLevel(0.)
{
	str = new char[1];
	str[0] = '\0';
}

CEqSig::CEqSig(int fs, const char *st)
:maxCalLevel(0.)
{
	sig.Reset(fs);
	str = new char[strlen(st)+1]; // to include the null char
	strcpy(str, st);
}

EXP_CS CEqSig::~CEqSig(void)
{
	delete[] str;
}

EXP_CS CEqSig::CEqSig(const CEqSig& src)
{
	str = new char[strlen(src.str)+1]; // to include the null char
	strcpy(str, src.str);
	sig = src.sig;
}

EXP_CS const CEqSig& CEqSig::operator=(const CEqSig& rhs)
{   
	if (this != &rhs) 
	{
		delete[] str;
		str = new char[strlen(rhs.str)+1]; // to include the null char
		strcpy(str, rhs.str);
		sig = rhs.sig;
	}
	return *this;
}

EXP_CS void CEqSig::setstr(const char *st)
{
	delete[] str;
	str = new char[strlen(st)+1]; // to include the null char
	strcpy(str, st);
}	

EXP_CS void CEqSig::SetmaxCalLevel(double level)
{
	maxCalLevel = level;
}

EXP_CS int CEqSig::DoesItHaveThis(int nTags, CEqSig *tags, const char *var)
{  //Returns 1 if contained, 0 if not contained, -1 if other error occurred
   // If the expression contains other variables, they should be defined and provided in tags; 
   // otherwise it may return -1 before it is evaluted against var.
	char buf[256];
	int res, id(-1);
	string boonsuk;

	CEqSig *_tags;

	if (nTags>0)
	{
		// Remove str from tags (if it exists)
		for (int i=0; i<nTags; i++)	if (tags[i].str==var) id=i;
		if (id>-1) nTags--;
		_tags = new CEqSig[nTags];
		for (int j(0), i(0); i<nTags; i++) if (i!=id) _tags[i] = tags[j++];
	}
	else
		_tags = tags;

	if (!Make(nTags, tags, buf))
	{
		if (strncmp(buf, UNKNOWNVAR, 30)==0)
		{
			boonsuk = buf;
			boonsuk = boonsuk.substr(boonsuk.find(":")+2);
			if (strstr(var, boonsuk.c_str())!=NULL)
				res= 1; // Yes, the expression contains var
			else				
				res = -1; // the expression has an error not related to var (--> it is going to err anyway).
		}
		else 
			res = -1; // the expression has an error not related to var (--> it is going to err anyway).
	}
	else
		res = 0; //it succeeded, so the expression does not contain var
	if (nTags>0) delete[] _tags;
	return res;
}

int CEqSig::CheckParenth()
{
	int count1, count2;
	char buf[256], *szbuffer;
	// strIn is parsed by plus/minus delimiters, then nsh, sh and plus of each sh are filled
	// then parsed by multi/div delimiters, then nph, ph and plus of each ph are filled 

	// First check if the parentheses are matched.
	count1 = howmanychr(str, '(');
	if ( count1 != howmanychr(str, ')')) 
	{sprintf(buf,"%s --> Unmatched parentheses ()", str); throw buf; }
	count2 = howmanychr(str, '[');
	if ( count2 != howmanychr(str, ']')) 
	{sprintf(buf,"%s --> Unmatched parentheses []", str); throw buf; }
	if ( count2 * count1 != 0) // if both () and [] exist, then check if these are crossed (one closed while the other is still open)
	{
		szbuffer = new char[strlen(str)+1];
		GetSurroundedBy('(', ')', str, szbuffer, 0);
		if ( howmanychr(szbuffer, '[') != howmanychr(szbuffer, ']'))
			{sprintf(buf,"%s --> Parentheses [] and () are crossed", szbuffer); delete[] szbuffer; throw buf; }
		delete[] szbuffer;
		return -1;
	}
	return 1;
}


int CEqSig::CountTerms(char delim)
{
	// 0 for error, 1 for one, ...
	char delim2;
	int depth(0), res(1);
	string _str(str);
	size_t lastloc(0), len;
	if (delim=='+') delim2='-';
	else if (delim=='*') delim2='/';
	else delim2 = delim;

	trim(_str,' ');	triml(_str,'+'); triml(_str,'-');
	// The above line should be trim(left... modify them later when you understand string class more

	len = _str.length();

	for (size_t i=0; i<len; i++)
	{
		switch (_str[i]) {
		case '[':
			for (depth=1; i+1<len && depth!=0; i++)
				depth += (_str[i+1]=='[') ? 1 : (_str[i+1]==']') ? -1 : 0;
			break;
		case '(':
			for (depth=1; i+1<len && depth!=0; i++)
				depth += (_str[i+1]=='(') ? 1 : (_str[i+1]==')') ? -1 : 0;
			break;
		case '"':
			for (depth=1; i+1<len && depth!=0; i++)
				depth += (_str[i+1]=='"') ? -1 : 0;
			break;
		case '\'':
			for (depth=1; i+1<len && depth!=0; i++)
				depth += (_str[i+1]=='\'') ? -1 : 0;
			break;
		case ']':
		case ')':
			return 0;	// Error. ] or ) is seen without preceding [ or (
		// depth is 0
		default:
			if (_str[i]==delim || _str[i]==delim2) { 
				if (i>lastloc)	res++;
				lastloc = i+1;
			}
		}
	}
	return (depth==0) ? res : 0;
}


int CEqSig::ParseTerms(char delim, string& term1, string& term2)
{
	int res(-1), depth(0);
	char delim2;
	string s ;

	// strIn is parsed and characters in delim are replaced with \0x1f if located outside of a parenthesis.
	// Returns -1 if parentheses are not matched. Otherwise, returns the number of items minus one deliminated 
	// (could be different from the number of replacement--due to 1) repetitive delim chars or 2) last char
	string _str(str);
	trim(_str,' ');
	int lastloc(-1);
	size_t len=_str.length();
	if (len==0) throw "Empty string (#1)";
	if (delim=='+') delim2='-';
	else if (delim=='*') delim2='/';
	else delim2 = delim;

	for (size_t i=0; i<len; i++) // beginning from 1
	{
		switch (_str[i]) {
		case '[':
			for (depth=1; i+1<len && depth!=0; i++)
				depth += (_str[i+1]=='[') ? 1 : (_str[i+1]==']') ? -1 : 0;
			break;
		case '(':
			for (depth=1; i+1<len && depth!=0; i++)
				depth += (_str[i+1]=='(') ? 1 : (_str[i+1]==')') ? -1 : 0;
			break;
		case '"':
			for (depth=1; i+1<len && depth!=0; i++)
				depth += (_str[i+1]=='"') ? -1 : 0;
			break;
		case '\'':
			for (depth=1; i+1<len && depth!=0; i++)
				depth += (_str[i+1]=='\'') ? -1 : 0;
			break;
		case ']':
		case ')':
			return 0;	// Error. ] or ) is seen without preceding [ or (
		// depth is 0
		default:
			if (_str[i]==delim || _str[i]==delim2) { 
				if (delim==',')
				{
					term1 = _str.substr(0,i);
					term2 = _str.substr(i+1, _str.length()-i-1);
					return 2;
				}
				else
					if (i>(size_t)(lastloc+1))	lastloc = (int)i;
			}
		}
	}
	if (lastloc>0)
	{
		term1 = _str.substr(0, (int)lastloc);
		term2 = _str.substr(lastloc);
		return 2;
	}
	else
	{
		term1 = _str.substr(0,string::npos);
		term2 = "";
		return 1;
	}
}


int CEqSig::StripParenthGetSign(char& wasParenth)
// Remove the out-most parentheses that follow the fist sign (+ or -)
// plus(+) or minus sign should appear the first after beginning white-spaces trim(med.
// Then strip the first parentheses encountered
{   // WHen this function is called, str should have been parsed
	// str (when called)       str (upated after this)   return value
    // 3						3								1
	// -3						3								-1
	// (3)						3								1
	// -(3)						3								-1
	// (-3)						-3								1
	// -(-3)					-3								-1
	// -sqrt(5)					sqrt(5)							-1
	// -(log(3))				log(3)							-1
	// (4+3					should not be called		not defined
	//This function should not be called in the following two cases
	// 2*log(3)											not defined
	// -2*log(3)										not defined
	// 2+log(3)											not defined

	bool sgn(true); // initialized positive
	string _str(str), buf;
	wasParenth = 0;
	trim(_str,' ');
	if (_str[0]=='-') sgn= false;
	// if the first non-white char is a sign char, remove it
	if (_str[0]=='+' || _str[0]=='-') _str = _str.substr(1, string::npos);
	//trim( again--- to take care of - (5+30)
	trim(_str,' ');
	//Now this first char should be an opening parentheis or quote
	switch (_str[0]) {
	case '(':
		if (_str[_str.length()-1]!=')')  { buf = _str + " --> Unmatched parentheses ()"; throw buf; }
		_str = _str.substr(1, _str.length()-2);
		wasParenth = '(';
		break;
	case '[':
		if (_str[_str.length()-1]!=']')  { buf = _str + " --> Unmatched brackets []"; throw buf; }
		_str = _str.substr(1, _str.length()-2);
		wasParenth = '[';
		break;
	case '"':
	case '\'':
		if (_str[_str.length()-1]!=_str[0])
		{
			buf = _str + " --> Unmatched quotes " + _str[0];
			throw buf;
		}
		wasParenth = _str[0];
		_str = _str.substr(1, _str.length()-2);
		break;
	}
	setstr(_str.c_str());
	return sgn ? 1: -1;
}

int CEqSig::RetrieveTag(int nTags, CEqSig *tags)
{
	// ASSUME THAT IT WAS LEFT trim(MED
	char buffer[512], *szbuffer;
	int jj, len;
	size_t id1, id2;
	double xx, *duffer;
	string name, args, term1, term2, out, _str(str), div[2];
	CSignals sig2(0); // to make it a duffer

	// parse name and indices (ids)-- by bracket
	CheckParenth(); // will be thrown
	id1 = _str.find("(");
	id2 = _str.find("[");

	if (id1==string::npos && id2==string::npos) name = _str;
	else		name = _str.substr(0, min(id1,id2)); // check when one of these is npos
	if (name.length()==0) return 0;

	// search if the tag name exists

	for (int i=0; i<nTags; i++)
	{
		if (name==tags[i].str) // if name is found from Tag list 
		{	
			if (id1==string::npos && id2==string::npos) sig = tags[i].sig; 
			else if (id2<id1) // if this is index  '['
			{
				CEqSig local(sig.GetFs(), _str.substr(id2+1, _str.length()-name.length()-2).c_str());
				if (local.ParseTerms(',', term1, term2) != 1) { sprintf(buffer, "invalid index: %s", args.c_str()); throw buffer;}
				local.Process(nTags, tags);
				//check if local.sig is all integer or integer array
				if (local.sig.nSamples==0) 
				{ 
					xx = local.sig.value(); 
					jj = (int)xx; 
					if ((double)jj!=xx) { sprintf(buffer, "index %s non-integer %lf", local.str, xx); 	throw buffer;}
					sig.SetValue(tags[i].sig.buf[jj]);
				}
				else 
				{
					len = local.sig.nSamples;
					for (int k=0; k<len; k++)
					{
						xx = local.sig.buf[k]; jj = (int)xx;
						if ((double)jj!=xx) { sprintf(buffer, "index %s non-integer %lf", local.str, xx); 	throw buffer;}
						int slskndf =  tags[i].sig.nSamples;
						if (jj>=tags[i].sig.nSamples) {sprintf(buffer,"Index %s exceeding the length of the defined tag array %s ", local.str, tags[i].str); throw buffer;}
					}
					duffer = new double[len];
					for (int k=0; k<len; k++)	duffer[k] = tags[i].sig.buf[(int)local.sig.buf[k]];
					sig2.SetBufferFromDoubleArray(duffer, len); // duffer values are memcopied
					delete[] duffer;
					sig = sig2;
				}
			}
			else // if this is '(' i.,e., transposition
			{
				CEqSig local(sig.GetFs());
				double tvalue[2];
				name = _str.substr(0, id1); trim(name, ' ');
				szbuffer = new char[_str.length()+10];
				GetSurroundedBy('(',')', _str.c_str(), szbuffer, 0);	trimLeft(szbuffer, " ");
				jj = str2array(div, 2, szbuffer, " :");
				if (jj<=1) 
				{ sprintf(buffer, "Error: %s (arguments must be two scalars separated by a colon)", szbuffer); delete[] szbuffer;  throw buffer;}
				delete[]  szbuffer; 
				for (int k=0; k<2; k++)
				{
					local.setstr(div[k].c_str());
					local.Process(nTags, tags);
					if ( (len = local.sig.nSamples)>1)
					{ sprintf(buffer, "Error: %s (argument must be scala)", div[k].c_str()); throw buffer;}
					tvalue[k] = local.sig.value();
				}
				tags[i].sig.Take(sig2, tvalue[0], tvalue[1]);
				sig = sig2;
			}
			return 1;
		}
		else // if name is not found 
		{
			// Function evaluation...do this later.

		}
	}
	return 0;
}

void GenerateTempTagName(string &name)
{
	char out[MAX_PATH];
	GetTempFileName ("", "_t_e_m_p_", 0, out);
	name = &out[1];
	name = name.erase(name.find(".tmp"), string::npos);
}

void CEqSig::series2sig(int nTags, CEqSig *tags, const char *instr, CSignals &carrier)
{ // Assuming no colon, only numbers or formulae with space deliminated
	CSignals temp(0);

	int nItems = countDeliminators(instr, " ,\t\n\r");
	char **comb = new char*[nItems];
	for (int i=0; i<nItems; i++) comb[i] = new char[strlen(instr)+1];
	if (str2array(comb, nItems, instr, " ,\t\r\n")!=nItems) 
	{ for (int i=0; i<nItems; i++) delete[] comb[i]; delete[] comb; throw "str2array malfunction in series2sig";}
	temp.Reset(-1);
	for (int i=0; i<nItems; i++)
	{
		CEqSig local(sig.GetFs(), comb[i]);
		try {	local.Process(nTags, tags); }
		catch (char *errmsg)
		{
			for (int i=0; i<nItems; i++) delete[] comb[i];
			delete[] comb;
			throw errmsg;
		}
		temp += &local.sig;
	}
	for (int i=0; i<nItems; i++) delete[] comb[i];
	delete[] comb; 
	carrier += &temp;
}

void* CEqSig::ReadArray(int nTags, CEqSig *tags)
{ 
	int estLength, minLength, nColons, nSemiColons;
	char **holder;
	bool last(false);
	double step_final;
	CSignals remant(0), out(sig.GetFs()), temp(sig.GetFs()), chan[4]; //PRESUMPTION: no more than 4 channels
	string str0(str), sub;

	//first of all, it should check semi-colon.
	if ((nSemiColons = countChar(str, ';', &estLength, &minLength))>1)
	{
		size_t id, lastid(0);
		for (int k=0 ; k<nSemiColons; k++)
		{
			id = str0.find(';');
			CEqSig local(sig.GetFs(), str0.substr(lastid, id).c_str());
			local.ReadArray(nTags, tags);
			chan[k] = local.sig;
			str0 = str0.substr(id+1);
			lastid = id;
		}
		for (int k(nSemiColons-1); k>0; k--) 
			chan[k-1].SetNextChan(&chan[k]);
		this->sig = chan[0];
	}
	else
	{
		//break down by colon
		nColons = countChar(str, ':', &estLength, &minLength);
		if (minLength==0) throw "Two successive colons"; 
		holder = new char*[nColons+1];
		for (int i=0; i<nColons; i++) holder[i] = new char[estLength+1];
		if (str2array(holder, nColons, str, ":")!=nColons) 
		{ for (int i=0; i<nColons; i++) delete[] holder[i]; delete[] holder; throw "str2array malfunction in ReadArray";}
		//breaking-down done
		//Now, we need to inspect whether this 2-colon or 3-colon.
		CEqSig local1(sig.GetFs());
		CEqSig local2(sig.GetFs());
		CEqSig local3(sig.GetFs());
		CEqSig leftofver(sig.GetFs());
		if (nColons==1) // no colon
		{
			try { series2sig(nTags, tags, holder[0], local1.sig); }
			catch (char *errmsg)
			{
				for (int i=0; i<nColons; i++) delete[] holder[i]; delete[] holder; 
				throw errmsg;
			}
			out = local1.sig;
		}
		else
		{
			for (int i=0; i<nColons-1; i++) 
			{
				//Rule: If the string between two colons contains "anything" other than numeric and space (and period)
				//      only the first element after evaluation will be taken for the step (if the length==1 or 2) or final value (if length>1)
				//     But if it has only numbers separated by space, then parse accordingly.

				try {
					if (local1.len()==0) series2sig(nTags, tags, holder[i], local1.sig); 
					series2sig(nTags, tags, holder[i+1], local2.sig);
					if (i<nColons-2) series2sig(nTags, tags, holder[i+2], local3.sig);
				}
				catch (char *errmsg)
				{
					for (int i=0; i<nColons; i++) delete[] holder[i]; delete[] holder; 
					throw errmsg;
				}

				if (local1.len()>1)
				{
					out.UpdateBuffer(local1.len()-1);
					memcpy((void*)out.buf, (void*)local1.sig.buf, sizeof(double)*(local1.len()-1));
				}

				if (local2.len()==1)
				{ // second item is single--it may be either the step or the final value.
					step_final = local2.sig.buf[0];
					if (local3.len()>0)
						colonednums(temp, local1.sig.buf[local1.len()-1], step_final, local3.sig.buf[0]);
					else
						colonednums(temp, local1.sig.buf[local1.len()-1], step_final);
					out += &temp;
					leftofver = local3;
					if (local3.len()>0) i++;
					last = (i==nColons-2);
				}
				else // this must be (local2.len()>1)
				{
					// second item is multiple--wrap up with the first value and store the rest for further processing
					step_final = local2.sig.buf[0];
					colonednums(temp, local1.sig.buf[local1.len()-1], step_final);
					out += &temp;
					leftofver = local2;
					last = (i==nColons-2);
				}
				if (last)// if this is not the last
				{
					if (leftofver.len()>1)
					{
						remant.UpdateBuffer(leftofver.len()-1);
						memcpy((void*)remant.buf, (void*)&leftofver.sig.buf[1], sizeof(double)*(leftofver.len()-1));
						out += &remant;
					}
				}
				else
				{
					if (leftofver.len()>2)
					{
						remant.UpdateBuffer(leftofver.len()-2);
						memcpy((void*)remant.buf, (void*)&leftofver.sig.buf[1], sizeof(double)*(leftofver.len()-2));
						out += &remant;
					}
					local1.sig.SetBufferFromDoubleArray(&leftofver.sig.buf[leftofver.len()-1], 1);
					local2.sig.Reset(sig.GetFs());
					local3.sig.Reset(sig.GetFs());
				}
			}
		}

		for (int i=0; i<nColons; i++) delete[] holder[i];
		delete[] holder;

		sig = out;
	}
	return sig.buf;
}

CSignals& CEqSig::Compute(int nTags, CEqSig *tags)
{
	CalcManager x;
	double xxx;
	int res, id, count(0);
	char *szbuffer, buf[256], parenth;
	string name, argpart, estr, terms[2];
	bool sgn(true), sigdone(false);

	sgn = (StripParenthGetSign(parenth) > 0);
	string str0(str);
	if (parenth == '(')
	{
		Process(nTags, tags);
		sigdone=true;
	}
	else if (parenth == '[')
	{
		Process(nTags, tags, true);
		sigdone=true;
	}
	else if (parenth == '"' || parenth == '\'')
	{
		size_t len = str0.length();
		sig.UpdateBuffer((int)len);
		for (size_t i=0, j=0; i<len; i++) {
			if (str0[i]==parenth) {
				if (str0[i+1]==parenth) {
					i++;
					sig.nSamples--;
				} else {
					sprintf (buf,"Unexpected %d: %s", parenth, str);
					throw buf;
				}
			}
			sig.buf[j++] = str0[i];
		}
		sigdone=true;
	}
	else if (IsAllNumeric(str0))	{
		res = sscanf(str0.c_str(),"%lf", &xxx);	
		if (res>=0) {	sig.SetValue(xxx);		sigdone=true;}
		else		{	sprintf (buf,"Invalid Numeric: %s", str0.c_str()); throw buf; }
	} 
	else if (RetrieveTag(nTags, tags))
	{
		sigdone=true; 
	}

	if (!sigdone)
	{
		str0 = str;
		// if not a vector, now this should be a function
		if ((id = (int)str0.find("("))==(int)string::npos) 
		{	sprintf (buf, UNKNOWNVAR); strcat(buf, str0.c_str()); throw buf; }
		name = str0.substr(0, id);
		trim(name, ' ');
		szbuffer = new char[str0.length()+1];
		GetSurroundedBy('(',')', str0.c_str(), szbuffer, 0);
		CEqSig arg(sig.GetFs(), szbuffer);
		delete[] szbuffer; 

		// sigma is handled above all other functions, because this is the only function that does not go into the calbase class
		if (name=="sigma") 	return handle_sigma(arg.str, nTags, tags);
		// Aug 2-08 For now, local is fixed array (limited by 16 arguments), it should be dynamically allocated
		CEqSig local[16];
		for (int i=0; i<16; i++) local[i].sig.Reset(sig.GetFs());
		while (arg.strlength()>0 && arg.ParseTerms(',', terms[0], terms[1])>0)
		{
			local[count].setstr(terms[0].c_str());
			local[count].Process(nTags, tags);
			x.AddArg(&local[count++]);
			arg.setstr(terms[1].c_str());
		}
		local[count].setstr("") ;	// to mark the end of arg list.
		if (name.length()==0)	throw "empty function name";
		else if (name=="rms" && count==2)
		{
			arg = local[count-1];
			//Make sure if SetmaxCalLevel has been called in the main application whenever "rms" is called
			arg.sig.SetValue(arg.sig.value() - maxCalLevel);
			x.ChangeLastArg(&arg);
		}
		else if (name=="$")	// $() requires tags. By jhpark 10/20/2009
			return handle_varvar(local, nTags, tags);
		sig = x.Calc(name.c_str(), sig.GetFs()).sig;
	}
	else if (!sgn)
	{
		x.ClearArg();
		CEqSig local(sig.GetFs());
		local.sig.SetValue(-1.);
		x.AddArg(&local);
		x.AddArg(this);
		local = x.Calc("*");
		sig = local.sig;
	}
	return sig;
}

CSignals& CEqSig::handle_sigma(char *argstr, int nTags, CEqSig *tags)
{
	string terms[4], buf[2], estr, indextagStr;
	CEqSig localEq, indextag, main(sig.GetFs()), *tags_temp;
	int count(0), ntags(nTags);

	localEq.setstr(argstr);
	while (localEq.ParseTerms(',', terms[2*count], terms[2*count+1])>1)
	{
		localEq.setstr(terms[count+1].c_str());
		if (count++>2) throw "sigma must have two arguments.\ne.g., sigma(i=0:2, tone(500,[300*i 300*i+100]))";
	}
	if (count==0) throw "sigma must have two arguments.\ne.g., sigma(i=0:2, tone(500,[300*i 300*i+100]))";
	localEq.sig.Reset(-1);
	// Get the local tag (index variable as a temporary tag valid only here
	str2array(buf, 2, terms[0].c_str(), "="); 
	trim(buf[0],' '); trim(buf[1],' ');
	indextagStr = buf[0];
	estr = buf[1];
	estr = '[' + estr + ']';
	localEq.setstr(estr.c_str());
	localEq.Process(nTags, tags);

	tags_temp = new CEqSig[nTags];
	for (int i=0; i<nTags; i++) tags_temp[i] = tags[i]; 
	// Modify tags with the index variable and call Compute()
	sig.Reset(sig.GetFs());
	indextag = localEq;
	AddTag(ntags, &tags_temp, &indextag, 0);
	tags_temp[0].setstr(indextagStr.c_str()); // update str (so that the tag variable, not the definition, is stored)
	for (int i=0; i<max(1, localEq.len()); i++)
	{
		double *valpt = localEq.sig.buf;
		if (valpt==NULL) tags_temp[0].sig.SetValue(localEq.sig.value());
		else			tags_temp[0].sig.SetValue(valpt[i]);
		main.setstr(terms[1].c_str());
		try { main.Process (ntags, tags_temp); }
		catch (char *errmsg)
		{
			delete[] tags_temp;
			throw errmsg;
		}
		sig += main.sig;
	}
	//RemoveTag is no longer necessary because only temporary tags (tags_temp) were used 
	delete[] tags_temp;
	return sig;
}

CSignals& CEqSig::handle_varvar(CEqSig *args, int nTags, CEqSig *tags)
{
	char buf[256];
	string varname;

	if (args[2].strlength() != 0)
		throw "Too many arguments.";

	varname = args[0].sig2str();
	if (args[1].strlength() != 0)
	{
		sprintf(buf, "%.0f", args[1].sig.value());
		varname += buf;
	}
	setstr(varname.c_str());
	if (!RetrieveTag(nTags, tags))
	{
		sprintf (buf, UNKNOWNVAR "%s", str);
		throw buf;
	}
	return sig;
}

CSignals& CEqSig::Process(int nTags, CEqSig *tags, bool fRegardBlank)
{
	// This parses a complex input strings and each parsed elements are calcualted in 
	CalcManager x;
	string name;
	int res;
	bool sgn(true);
	char parenth;
	string term1, term2, dummy1, dummy2;
	CEqSig local1(sig.GetFs());
	CEqSig local2(sig.GetFs());

	//check if it is a string
	if (str[0]=='\0') throw "Empty string";
//	if (IsString(str.c_str())) return this->sig;

	CheckParenth();
	if ( CountTerms(':') == 1)
	{
		if (!fRegardBlank || CountTerms(' ') == 1)
		{
			if ( CountTerms('+') == 1)
			{
				// strip only if there's one * term.
				if ( CountTerms('*') == 1)
				{
					if (!(strchr(str, '^') !=NULL && ParseTerms('^', term1, term2)!=1))
					{
						return Compute(nTags, tags);
					}
					else
					{
						sgn = (StripParenthGetSign(parenth) > 0); // do I need sgn here?
						res = ParseTerms('^', term1, term2); //res must be 2
						if (res!=2) throw "Invalid format for ^: x raised by y (such as 2^3 =8)";
						local1.setstr(term1.c_str());
						local1.Process(nTags, tags);
						term2=term2.substr(1, string::npos);
						local2.setstr(term2.c_str());
						local2.Process(nTags, tags);
						x.AddArg(&local1);
						x.AddArg(&local2);
						local2 = x.Calc("^");
						this->sig = local2.sig;
						if (!sgn)
						{
							CEqSig local0(sig.GetFs());
							local0.sig.SetValue(-1.);
							x.ClearArg();
							x.AddArg(&local0);
							x.AddArg(this);
							local0 = x.Calc("*");
							this->sig = local0.sig;
						}
						return this->sig;
					}
				}
				else
				{
					res = ParseTerms('*', term1, term2); //res must be greater than 2
					local1.setstr(term1.c_str());
					local1.Process(nTags, tags);
					if (term2[0]=='/') sgn = false;
					term2=term2.substr(1, string::npos);
					local2.setstr(term2.c_str());
					local2.Process(nTags, tags);
					x.AddArg(&local1);
					x.AddArg(&local2);
					if (sgn)	local2 = x.Calc("*");
					else		local2 = x.Calc("/");
				}
			}
			else
			{
				res = ParseTerms('+', term1, term2); 
				local1.setstr(term1.c_str());
				local1.Process(nTags, tags);
				local2.setstr(term2.c_str());
				local2.Process(nTags, tags);
				x.AddArg(&local1);
				x.AddArg(&local2);
				local2 = x.Calc("+");
			}
		}
		else if (fRegardBlank)
		{
			res = ParseTerms(' ', term1, term2); 
			local1.setstr(term1.c_str());
			local1.Process(nTags, tags, true);
			local2.setstr(term2.c_str());
			local2.Process(nTags, tags);
			x.AddArg(&local1);
			x.AddArg(&local2);
			local2 = x.Calc(" ");
		}
	}
	else
	{
		ReadArray(nTags, tags);
		return this->sig;
	}
	this->sig = local2.sig;
	return this->sig;
}

/*
What is the difference between Make() and Compute()?

Make() : evaluate the whole string that was not parsed (parse and compute)
Compute() : evaluate a parsed term
Process() : 

If + , - , * or / operator is still in str, Make, not Compute, should be called  
*/
EXP_CS int CEqSig::Make(int nTags, CEqSig *tags, char *errstr)
{
	//from a code using string object for errstr...now take care of handing len.. at your earliest convenience
	int count(0), res(1);
	bool addbracket(false);
	int logicoperation(false);
	string lhs, rhs, oper, brac, str1, str2;
	CEqSig lhsig(-1);
	CSignals out;
	logicoperation = ParseLRHS(lhs, rhs, oper);
	if (logicoperation)
	{
		lhsig.setstr(lhs.c_str());
		trim(lhs, " ");
		if (logicoperation==1)
		{
			if (!lhsig.Make(nTags, tags, errstr)) return 0;
			if (lhsig.len()>1) {strcpy(errstr, "Evaluted LHS should be a scala"); return 0;}
		}
		setstr(rhs.c_str());
	}
	CEqSig *copiedTags = new CEqSig[nTags];
	for (int i=0; i<nTags; i++) copiedTags[i] = tags[i];
	/* the following code should move to psynteg
	addbracket = oper.length()==0 && str.find(':') != std::string.npos && str.find('[') == std::string.npos && str.find("sigma")== std::string.npos;
	if (addbracket)
	{
		brac = '[' + str;
		brac += ']';
		str = brac;
	}
	*/
	try
	{
		Process(nTags, copiedTags);
		errstr[0] = 0;
		if (logicoperation) setstr(lhs.c_str());
	}
	catch (char *errmsg)
	{
		res=0;
		strcpy(errstr, errmsg); 
	}
	if (logicoperation==1)
	{
		if (len()>1) { strcpy(errstr, "Evaluted RHS should be a scala"); return 0;}

		CSignals value;
		if (oper==">")	out.SetValue((bool)(lhsig.sig.value() > sig.value()));
		else if (oper=="<")	out.SetValue((bool)(lhsig.sig.value() < sig.value()));
		else if (oper==">=")	out.SetValue((bool)(lhsig.sig.value() >= sig.value()));
		else if (oper=="<=")	out.SetValue((bool)(lhsig.sig.value() <= sig.value()));
		else if (oper=="!=")	out.SetValue((bool)(lhsig.sig.value() != sig.value()));
		else if (oper=="==")	out.SetValue((bool)(lhsig.sig.value() == sig.value()));
		else { strcpy(errstr, "Unrecognized operator--internal error3345 in EqSig.cpp"); return 0;}
		sig = out;
	}
	delete[] copiedTags;
	return res;
}

EXP_CS void *CEqSig::Play(int devID, char *errstr, int len)
{
	return sig.PlayArray(devID, errstr);
}

EXP_CS int CEqSig::GetNonZeroPt()
{
	int nzPoint(0);
	for (int i=0; i<sig.GetLength(); i++)
		if (sig.buf[i]!=0) {nzPoint = i;	return nzPoint;}
	return sig.GetLength()-1;
}

void CEqSig::FileWithPath(string strIn, string& strOut, const char* defaultExt)
{//This function is only called by cal_wave::calc()
	// The only reason that this function exists as a member function is to use theApp
	char quot('"'), drv[_MAX_DRIVE], dir[_MAX_DIR], name[_MAX_FNAME], ext[_MAX_EXT];
	string path;
	char *_strIn = new char[strIn.length()+1];

	strcpy(_strIn, strIn.c_str());
	_splitpath(_strIn, drv, dir, name, ext);
	/*if (drv[0]=='\0' && dir[0]=='\0') // File name only... add current path
		strOut = appPath + _strIn;
	else
	*/	strOut = _strIn;
	if (ext[0]=='\0')
	{
		sprintf(name, ".%s", defaultExt);
		strOut += name ;
	}
	delete[] _strIn;
}

string CEqSig::sig2str()
{
	string out;
	out.resize(sig.nSamples);
	for (int i=0; i<sig.nSamples; i++)
		out[i] = (char)sig.buf[i];
	return out;
}