/*
 GTfold: compute minimum free energy of RNA secondary structure
 Copyright (C) 2008  David A. Bader
 http://www.cc.gatech.edu/~bader

 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/>.
 
 @author prashant {pgaurav@gatech.edu}
 
 */

#ifndef _SUBOPT_TRACEBACK_H
#define _SUBOPT_TRACEBACK_H

#include <cassert>
#include <iostream>
#include <sstream>
#include <stack>
#include <map>
#include <vector>


#define Ed3(x, y, z)  dangle[RNA[x]][RNA[y]][RNA[z]][1]
#define Ed5(x, y, z)  dangle[RNA[x]][RNA[y]][RNA[z]][0]
#define auPenalty(x, y) auPen(RNA[x], RNA[y]) 
#define Ea multConst[0]
#define Eb multConst[2]
#define Ec multConst[1] 

#define V(x,y)	V[indx[x]+y]
#define TURN 3

enum label {lW=0, lV, lVBI, lVM, lWM};
extern const char* lstr[]; 

struct segment 
{
	int i_;
	int j_;
	int label_;
	int en_;

	segment(int i, int j, enum label lbl, int en) 
	{
		i_= i; j_ = j;
		label_ = lbl;
		en_= en; 
	}

	segment(const segment& ss)
	{
		i_ = ss.i_;
		j_ = ss.j_;
		label_ = ss.label_;
		en_ = ss.en_;
	}

	segment& operator = (const segment& ss)
	{
		if (this != &ss)
		{
			i_ = ss.i_;
			j_ = ss.j_;
			label_ = ss.label_;
			en_ = ss.en_;	
		}
		return *this;
	}
	
	friend std::ostream& operator<<(std::ostream& out, const segment& seg)
	{
		out << "<(" << seg.i_ << ',' << seg.j_ << ')' << " " 
			<< lstr[seg.label_] << " dG=" << seg.en_ << ">";
		return out;
	}
};

typedef segment SEG;
typedef std::stack<segment>  SEGSTACK;

struct pstruct
{
	std::string str;
	SEGSTACK st_segment;
	SEGSTACK st_v; /* used for backtracking in traceWM */
	
	int ae_;
	int le_;

	int total() const { return ae_ + le_; }

	pstruct() {}
	
	pstruct(const pstruct& ps)
	{
		str = ps.str;
		st_segment = ps.st_segment;
		st_v = ps.st_v;
		ae_ = ps.ae_;
		le_ = ps.le_;
	}

	pstruct& operator = (const pstruct& ps)
	{
		if (&ps != this)
		{
			str = ps.str;
			st_segment = ps.st_segment;
			st_v = ps.st_v;
			ae_ = ps.ae_;
			le_ = ps.le_;
		}
		return *this;
	}


	void update(int i, int j, char c1, char c2)
	{
		str[i-1] = c1; str[j-1] = c2;	
	}

	pstruct(int ae, int len) : ae_(ae) 
	{
		le_ = 0;
		str = std::string(len, '.');
	}

	void accumulate(int en)
	{
		ae_ += en;
	}

	void push(segment seg) 
	{
		st_segment.push(seg);
		le_ += seg.en_;
	}

	void push_v(segment seg) 
	{
		st_v.push(seg);
		le_ += seg.en_;
	}

	void pop()
	{
		segment seg = st_segment.top();
		le_ -= seg.en_;
		st_segment.pop();
	}

	void pop_v()
	{
		segment seg = st_v.top();
		le_ -= seg.en_;
		st_v.pop();
	}

	segment top()
	{
		return st_segment.top();
	}


	segment top_v()
	{
		return st_v.top();
	}

	bool empty() 
	{
		return st_segment.empty();
	}

	bool empty_v() 
	{
		return st_v.empty();
	}

	void print()
	{
		SEGSTACK st = st_segment;
		std::cout <<'[' << ' ' ;
		while (!st.empty())
		{
			segment seg = st.top();
			std::cout << seg << ' '; 
			st.pop();	
		}

		st = st_v;
		std::cout << ']' << ' ' << '[' ;
		while (!st.empty())
		{
			segment seg = st.top();
			std::cout << seg << ' '; 
			st.pop();	
		}
		std::cout << ']' << ' ' << str << ' ' ;
		std::cout << " ae=" << ae_ << " le=" << le_ << " te=" << ae_+le_  ;
	}
};

typedef pstruct PS;
typedef std::stack<pstruct> PSSTACK;
typedef std::map<std::string, pstruct> PSMAP;

void push_to_gstack(PSSTACK& gs, const PS& v);
void subopt_traceback(int len, int gap);
void traceV(int i, int j, PS& ps, PSSTACK& gs, int energy); 
void traceVBI( int i, int j, PS& ps, PSSTACK& gs, int energy);
void traceW( int i, int j, PS& ps, PSSTACK& gs, int energy);
void traceVM( int i, int j, PS& ps, PSSTACK& gs, int energy);
void traceWM(PS& ps, PSMAP& filter);

//enum label {lW=0, lV, lVBI, lVM, lWM};

#endif