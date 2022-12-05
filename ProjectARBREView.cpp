
// ProjectARBREView.cpp : implementation of the CProjectARBREView class
//

#include "pch.h"
#include "framework.h"
#include <stack>
// SHARED_HANDLERS can be defined in an ATL project implementing preview, thumbnail
// and search filter handlers and allows sharing of document code with that project.
#ifndef SHARED_HANDLERS
#include "ProjectARBRE.h"
#endif

#include "ProjectARBREDoc.h"
#include "ProjectARBREView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CProjectARBREView

IMPLEMENT_DYNCREATE(CProjectARBREView, CView)

BEGIN_MESSAGE_MAP(CProjectARBREView, CView)
	ON_WM_KEYDOWN()
END_MESSAGE_MAP()

//******************************************************************************************
//FlgDirection
#define PRINCIPAL 0x0001 // 1
#define GAUCHE 0x0002  // 2
#define DROITE 0x0004  // 4
//FlgFnDeb
#define DEBUT 0x0008  // 8
#define FIN 0x0010  // 16
//FlgFtAvRec
#define FAIT 0x0020  // 32
#define RECULE 0x0040  // 64
#define AVANCE 0x0080  // 128

#define SCALE 1		// inverse
#define LNG 1		//longeur de feuille
#define LONG_BRANCHE 150.0/SCALE
#define EPAISSEUR_MIN 2.0/SCALE
#define DECREMENT_EPAISSEUR 1.0/SCALE
#define PROBABILITE 12

class Position
{
private:
	CPoint PosDepart;
	CPoint PosArrive;
	int AngleCourante;
	COLORREF Couleur;
	float Epaisseur;
	int Niveau;

public:
	int GetNiveau() { return Niveau; }
	CPoint GetPosDepart() { return PosDepart; }
	CPoint GetPosArrive() { return PosArrive; }
	float GetEpaisseur() { return Epaisseur; }
	int GetAngleCourante() { return AngleCourante; }
	COLORREF GetCouleur() { return Couleur; }
	/********************************************************/
	void SetNiveau(int lv) { Niveau = lv; }
	void IncNiveau(int Ilv) { Niveau += Ilv; }
	void SetPosDepart(CPoint dp) { PosDepart = dp; }
	void SetPosArrive(CPoint pa) { PosArrive = pa; }
	void SetEpaisseur(float e) { Epaisseur = e; }
	void SetAngleCourante(int a) { AngleCourante = a; }
	int IncAngleCourante(int a) { AngleCourante = AngleCourante + a; return AngleCourante; }
	void SetCouleur(COLORREF col) { Couleur = col; }
	void SetCouleur() { Couleur = RGB(196, 64, 0); }
	/********************constructeur*******************/
	Position() {
		PosDepart.x = 0; PosDepart.y = 0;
		PosArrive.x = 0;	PosArrive.y = 0; Niveau = 1;
		Epaisseur = 35.0/SCALE; AngleCourante = 35;  Couleur = RGB(196, 64, 0);
	}

};
class node : public Position
{
private:
	node* child1;
	node* child2;
	node* child3;
	node* parent;
	int pourCentExistance;
public:
	int  NoDebug;
	unsigned FlgDirection;
	unsigned FlgFnDeb;
	unsigned FlgFtAvRec;
public:
	bool SetChild1(node* chd1) {
		if (child1 != nullptr) return false;
		else { child1 = chd1; return true; }
	}
	bool SetChild2(node* chd2) {
		if (child2 != nullptr) return false;
		else { child2 = chd2; return true; }
	}
	bool SetChild3(node* chd3) {
		if (child3 != nullptr) return false;
		else { child3 = chd3; return true; }
	}
	bool SetParent(node* p) {
		if (parent != nullptr) return false;
		else { parent = p; return true; }
	}
	node* GetChild1() { return child1; }
	node* GetChild2() { return child2; }
	node* GetChild3() { return child3; }
	node* GetParent() { return parent; }

	bool SetPcEXistance(int pc) {
		if (pc > 100)  return false;
		else { pourCentExistance = pc;  return true; }
	}
	int GetPcExistance() { return pourCentExistance; }

	// constructor new node 3 childandparent set to null
	node(node* prt) {
		child1 = nullptr; child2 = nullptr; child3 = nullptr;  parent = prt;
		NoDebug = 0;  pourCentExistance = 100;
		FlgDirection = PRINCIPAL; FlgFnDeb = 0x0000; FlgFtAvRec = 0x0000;
	};
	node() {
		child1 = nullptr; child2 = nullptr; child3 = nullptr;  parent = nullptr;
		NoDebug = 0;  pourCentExistance = 100;
		FlgDirection = PRINCIPAL; FlgFnDeb = 0x0000; FlgFtAvRec = 0x0000;
	};
};
bool GetProbability(int n) { if ((100 * rand() / RAND_MAX) < n) return true; else return false; }

float sinD(float f) { return sin(f * 3.1416 / 180.0); };
float cosD(float f) { return cos(f * 3.1416 / 180.0); };

int GetMinMax(int min, int max) { return (((rand() * (max - min) / RAND_MAX) + min)); }
void MyOut(int x, int y, CString st, int c, CDC* pDC);
bool dessineFeuille(node* n,int XXX,int YYY, CDC* pDC)
{
	if (n->GetEpaisseur() > 4) return true;
	CPen pn(PS_SOLID, 0, RGB(0, 255, 0));
	CBrush br(RGB(0, 255, 0));
	//	static int f = 0;  if (f > 0) return true;
//	f++;
	pDC->SelectObject(&pn);
	pDC->SelectObject(&br);
	int Xa = (n->GetPosDepart().x / SCALE) + XXX; //350
	int Ya = (n->GetPosDepart().y / SCALE) + YYY; //580

	CPoint FPt1[8];
	CPoint FPt2[8];
	CPoint FPt3[8];
	CPoint FPt4[8];
	CPoint FPt5[8];
	CPoint FPt6[8];
	CPoint FPt7[8];
	CPoint FPt8[8];
	float U = 4;

	FPt1[0].x = Xa + U;
	FPt1[1].x = Xa + 5.0 * U / 3.0;
	FPt1[2].x = Xa + 7.0 * U / 3.0;
	FPt1[3].x = Xa + 3.0 * U;
	FPt1[4].x = Xa + 11.0 * U / 3.0;

	FPt1[5].x = Xa + 3.0 * U;
	FPt1[6].x = Xa + 7.0 * U / 3.0;
	FPt1[7].x = Xa + 5.0 * U / 3.0;
	/**********************************************************************************/
	FPt1[0].y = Ya;
	FPt1[1].y = Ya + 2.0 * U / 3.0;
	FPt1[2].y = Ya + U / 2.0;
	FPt1[3].y = Ya + 2.0 * U / 3.0;
	FPt1[4].y = Ya;

	FPt1[5].y = Ya - 2.0 * U / 3.0;
	FPt1[6].y = Ya - U / 2.0;
	FPt1[7].y = Ya - 2.0 * U / 3.0;

	float Ag = -45.0;  //rotate x degrees
	/**********************************************************************************/
	for (int i = 0; i < 8; i++) FPt2[i].x = Xa + cosD(Ag) * (FPt1[i].x - Xa) - sinD(Ag) * (FPt1[i].y - Ya);
	/**********************************************************************************/
	for (int i = 0; i < 8; i++) FPt2[i].y = Ya + sinD(Ag) * (FPt1[i].x - Xa) + cosD(Ag) * (FPt1[i].y - Ya);
	/**********************************************************************************/
	Ag = -90.0;  //rotate x degrees
	/**********************************************************************************/
	for (int i = 0; i < 8; i++) FPt3[i].x = Xa + cosD(Ag) * (FPt1[i].x - Xa) - sinD(Ag) * (FPt1[i].y - Ya);
	/**********************************************************************************/
	for (int i = 0; i < 8; i++) FPt3[i].y = Ya + sinD(Ag) * (FPt1[i].x - Xa) + cosD(Ag) * (FPt1[i].y - Ya);
	/**********************************************************************************/
	Ag = -135.0;  //rotate x degrees
	/**********************************************************************************/
	for (int i = 0; i < 8; i++) FPt4[i].x = Xa + cosD(Ag) * (FPt1[i].x - Xa) - sinD(Ag) * (FPt1[i].y - Ya);
	/**********************************************************************************/
	for (int i = 0; i < 8; i++) FPt4[i].y = Ya + sinD(Ag) * (FPt1[i].x - Xa) + cosD(Ag) * (FPt1[i].y - Ya);
	/**********************************************************************************/
	Ag = 45.0;  //rotate x degrees
	/**********************************************************************************/
	for (int i = 0; i < 8; i++) FPt5[i].x = Xa + cosD(Ag) * (FPt1[i].x - Xa) - sinD(Ag) * (FPt1[i].y - Ya);
	/**********************************************************************************/
	for (int i = 0; i < 8; i++) FPt5[i].y = Ya + sinD(Ag) * (FPt1[i].x - Xa) + cosD(Ag) * (FPt1[i].y - Ya);
	/**********************************************************************************/
	Ag = 90.0;  //rotate x degrees
	/**********************************************************************************/
	for (int i = 0; i < 8; i++) FPt6[i].x = Xa + cosD(Ag) * (FPt1[i].x - Xa) - sinD(Ag) * (FPt1[i].y - Ya);
	/**********************************************************************************/
	for (int i = 0; i < 8; i++) FPt6[i].y = Ya + sinD(Ag) * (FPt1[i].x - Xa) + cosD(Ag) * (FPt1[i].y - Ya);
	/**********************************************************************************/
	Ag = 135.0;  //rotate x degrees
	/**********************************************************************************/
	for (int i = 0; i < 8; i++) FPt7[i].x = Xa + cosD(Ag) * (FPt1[i].x - Xa) - sinD(Ag) * (FPt1[i].y - Ya);
	/**********************************************************************************/
	for (int i = 0; i < 8; i++) FPt7[i].y = Ya + sinD(Ag) * (FPt1[i].x - Xa) + cosD(Ag) * (FPt1[i].y - Ya);
	/**********************************************************************************/
	Ag = 180.0;  //rotate x degrees
	/**********************************************************************************/
	for (int i = 0; i < 8; i++) FPt8[i].x = Xa + cosD(Ag) * (FPt1[i].x - Xa) - sinD(Ag) * (FPt1[i].y - Ya);
	/**********************************************************************************/
	for (int i = 0; i < 8; i++) FPt8[i].y = Ya + sinD(Ag) * (FPt1[i].x - Xa) + cosD(Ag) * (FPt1[i].y - Ya);
	/**********************************************************************************/

	/*	CRect Rect(0, 0, 700, 600);
		pDC->SetMapMode(MM_LOMETRIC)
		pDC->DPtoLP(&Rect);
		*/
	int Iter = GetMinMax(0, 5);

	for(int h=0;h<Iter;h++)
	{
			int CCAS = GetMinMax(0, 9);
			switch (CCAS)
			{
			case 1: {	pDC->Polygon(FPt1, 8); break; }
			case 2: {   pDC->Polygon(FPt2, 8); break; }
			case 3: {   pDC->Polygon(FPt3, 8); break; }
			case 4: {   pDC->Polygon(FPt4, 8); break; }
			case 5: {   pDC->Polygon(FPt5, 8); break; }
			case 6: {   pDC->Polygon(FPt6, 8); break; }
			case 7: {   pDC->Polygon(FPt7, 8); break; }
			case 8: {   pDC->Polygon(FPt8, 8); break; }
			default: break;
			};
	}
/*	MyOut(FPt1[1].x, FPt1[1].y, CString("P1x=%d<"), int(sin((45 + 21.8) * 3.14 / 180) * (U * sqrt(29) / 3.0)), pDC);
	MyOut(FPt1[2].x, FPt1[2].y, CString("P2x=%d<"), int(sin((45 + 12.1) * 3.14 / 180) * (U * sqrt(205) / 6.0)), pDC);
	MyOut(FPt1[3].x, FPt1[3].y, CString("P3x=%d<"), int(sin((45 + 12.5) * 3.14 / 180) * (U * sqrt(85) / 3.0)), pDC);
	MyOut(FPt1[4].x, FPt1[4].y, CString("P4x=%d<"), int(sin(45.0 * 3.14 / 180) * (U * 11 / 3.0)), pDC);
	*/
	return false;

}
void MyOut(CString st, int c, CDC* pDC)
{
	static int i = 0; static int j = 0;
	CString str;
	str.Format(st, c);
	if (i > 800) {
		i = 0; j += 20;
	}
	pDC->TextOutW(400, 50, str);
	i += 150;
}
void MyOut(int x,int y,CString st, int c, CDC* pDC)
{
	static int i = 0; static int j = 0;
	CString str;
	str.Format(st, c);
	if (i > 800) {
		i = 0; j += 20;
	}
	pDC->TextOutW(x, y, str);
	i += 150;
}
void SetPosAng(node* n,int XXX,int YYY,CDC* pDC)
{
	dessineFeuille(n,XXX,YYY,pDC);
	switch (n->FlgDirection) {
	case GAUCHE: {
		node* Par = n->GetParent();
		if (Par != nullptr) {
			n->SetEpaisseur(Par->GetEpaisseur() * 2.0 / 3.0);
			n->SetAngleCourante(Par->GetAngleCourante()-30);
		}
		else {
			n->SetEpaisseur(n->GetEpaisseur()*2.0 / 3.0);
			n->SetAngleCourante(20);
		}
		break; }
	case PRINCIPAL: {
		node* Par = n->GetParent();
		if (Par != nullptr) {
			n->SetAngleCourante(Par->GetAngleCourante()+ GetMinMax(1, 12)- GetMinMax(1, 12));
			n->SetEpaisseur(Par->GetEpaisseur() - DECREMENT_EPAISSEUR);
		}
		else {
			n->SetAngleCourante(0);
			n->SetEpaisseur(n->GetEpaisseur() - DECREMENT_EPAISSEUR);
		}
		break; }
	case DROITE: {
		node* Par = n->GetParent();
		if (Par != nullptr) {
			n->SetAngleCourante(Par->GetAngleCourante()+30);
			n->SetEpaisseur(Par->GetEpaisseur() * 2 / 3);
		}
		else {
			n->SetAngleCourante(-20);
			n->SetEpaisseur(n->GetEpaisseur() * 2 / 3);
		}
		break; }
	default: {
		pDC->TextOut(100, 300, CString("ERROR SWITCH IN setPosAng"));
		return; }
	}

	node* Par = n->GetParent();
	if (Par != nullptr) {
		n->SetPosDepart(Par->GetPosArrive());
	}
	else {
//		n->SetEpaisseur(n->GetEpaisseur()/2);
		n->SetPosDepart(CPoint(0, 0));
	}
	return;
}
/*bool Dessine(node* n, int XXX, int YYY, CDC* pDC)
{

	static bool fl = true;
	if (fl) { fl = false; }
	else return fl;

	//	MyOut(CString("Epai = %d"),n->GetEpaisseur(), pDC);


	int XX = 0; int YY = 0; int X = 0; int Y = 0;

			node* Par = n->GetParent();
			if (Par != nullptr) {
				XX = Par->GetPosArrive().x;
				YY = Par->GetPosArrive().y;
			}
			else {
				XX = YY = 0;
			}
	
	int LongBranche = LONG_BRANCHE;
	for (int ext = 0; ext < 100; ext += 20)
	{

		int xf = LongBranche * cos(-90 + n->GetAngleCourante() * 3.14 / 180);
		int yf = LongBranche * sin(-90 + n->GetAngleCourante() * 3.14 / 180);
		X = n->GetPosDepart().x + xf + ext * cos(75 * 3.14 / 180) / tan(75 * 3.14 / 180);
		Y = /*n->GetPosDepart().y + yf + ext * cos(75 * 3.14 / 180);
		XX = 50 + ext; YY = 50 + ext;
		n->SetPosArrive(CPoint(X, Y));	//check ca 
				for (int cl = 0; cl < n->GetEpaisseur(); cl++)

					int total = 75 + cl * 3;
					CPen pn(PS_SOLID, 0, RGB(total*0.66, total*0.34, total*0.0));
					pDC->SelectObject(&pn);
		//			cl/tan((n->GetAngleCourante()) * 3.14 / 180)
						pDC->MoveTo(XX / SCALE + 300, YY / SCALE + 200); pDC->LineTo(X / SCALE + 300, Y / SCALE + 200);
		//			pDC->MoveTo(400+cl, 400); pDC->LineTo(400+cl,500);

		//		}
	}
	return true;

}*/
bool Dessine(node* n, int XXX, int YYY, CDC* pDC)
{

	static bool fl = true;
	if (fl) { fl = false; }
	else return fl;

	//	MyOut(CString("Epai = %d"),n->GetEpaisseur(), pDC);


	int XX = 0; int YY = 0; int X = 0; int Y = 0;

	/*		node* Par = n->GetParent();
			if (Par != nullptr) {
				XX = Par->GetPosArrive().x;
				YY = Par->GetPosArrive().y;
			}
			else {
				XX = YY = 0;
			}
	*/
	int LongBranche = LONG_BRANCHE;
	for (int ext = 0; ext < 100; ext += 1)
	{
		int angle = 10;

		int xf = LongBranche * cos(-90 + n->GetAngleCourante() * 3.14 / 180);
		int yf = LongBranche * sin(-90 + n->GetAngleCourante() * 3.14 / 180);
		X = /*n->GetPosDepart().x*/150 + xf + ext * cos(angle * 3.14 / 180) / tan(angle * 3.14 / 180);
		Y = /*n->GetPosDepart().y */150 + yf + ext * cos(angle * 3.14 / 180);
		XX = 50 + ext; YY = 50 + ext;
		n->SetPosArrive(CPoint(X, Y));	//check ca 
				for (int cl = 0; cl < 50000000; cl++);
				{
					int total = 55 + ext * 2;
					CPen pn(PS_SOLID, 0, RGB(total, total*0.34, total*0.0));
					pDC->SelectObject(&pn);
		///			cl/tan((n->GetAngleCourante()) * 3.14 / 180)
					pDC->MoveTo(XX / SCALE + 300, YY / SCALE + 200); pDC->LineTo(X / SCALE + 300, Y / SCALE + 200);
		//			pDC->MoveTo(400+cl, 400); pDC->LineTo(400+cl,500);

				}
	}
	return true;

}

bool InitialeNode(node* n, node* parent, CDC* pDC)
{
	static int x = 0; static int y = 0;

	if (n->GetEpaisseur() > EPAISSEUR_MIN)
	{
		n->SetCouleur();//met la couleur initial
		n->SetPcEXistance(99); // check dans dessine
		if (parent != nullptr) {
			n->SetEpaisseur(parent->GetEpaisseur() - DECREMENT_EPAISSEUR);
			n->SetPosDepart(parent->GetPosArrive());
		}
		else {
			n->SetPosDepart(n->GetPosArrive());
			n->SetEpaisseur(n->GetEpaisseur() - DECREMENT_EPAISSEUR);
		}
	}
//	MyOut(CString("epais %d<<"), n->GetEpaisseur(), pDC);

	return true;
}
std::stack<node*> mem;
node* CreateNode(node* n, unsigned flags, CDC* pDC)
{
	static int x = 0; static int y = 0;

	static int noBug = 1;

	switch (flags) {
	case GAUCHE: {
		node* temp1 = new node(n);
		mem.push(temp1);
		n->SetChild1(temp1);
		InitialeNode(temp1, n, pDC);
		temp1->NoDebug = noBug++;
//				MyOut(100,100,CString("no=%d<"), temp1->NoDebug, pDC);
		return temp1;
		break; }
	case PRINCIPAL: {
		node* temp2 = new node(n);
		mem.push(temp2);
		n->SetChild2(temp2);
		InitialeNode(temp2, n, pDC);
		temp2->NoDebug = noBug++;
//				MyOut(100,100,CString("no=%d<"), temp2->NoDebug, pDC);
		return temp2;
		break; }
	case DROITE: {
		node* temp3 = new node(n);
		n->SetChild3(temp3);
		mem.push(temp3);
		InitialeNode(temp3, n, pDC);
		temp3->NoDebug = noBug++;
//				MyOut(100,100,CString("no=%d<"), temp3->NoDebug, pDC);
		return temp3; //return new node
		break; }
	default: {
		pDC->TextOut(100, 300, CString("ERROR SWITCH IN CREATE"));
		break; }
	}
	return nullptr;
}

bool GoRevientNode(node* e_node,int XXX,int YYY, CDC* pDC);

bool GoDrawOneNode(node* e_node,int XXX,int YYY, CDC* pDC)
{
	static int x = 0; static int y = 0;
	x += 20; y += 20;

	static int compteur_new = 1;

	node* this_node = e_node;
	node* child_node = e_node;
	node* Par_node = this_node->GetParent();
	if ((Par_node == nullptr) && (this_node->FlgFnDeb == DEBUT)) return true; //fin de la fonction recursive fin de l'arbre
	while (this_node->GetEpaisseur() > EPAISSEUR_MIN)
	{
//		compteur_new += 1; y += 20;
//		if (compteur_new > 1000000) return 1;//maximum no of call to the recursive function
//		MyOut(150,600,CString("comp=%d<"), compteur_new, pDC);

		if ((this_node->GetChild1() != nullptr) && (this_node->GetChild3() != nullptr)
			&& (this_node->GetChild2() != nullptr)) break;

		if (this_node->GetChild1() == nullptr)
		{
			child_node = CreateNode(this_node, GAUCHE, pDC); // check si unebranche a gauche
			child_node->FlgDirection = GAUCHE;

			if (GetProbability(PROBABILITE))
			{
				child_node->FlgFnDeb = DEBUT;
				SetPosAng(child_node,XXX,YYY,pDC);
				Dessine(child_node,XXX,YYY, pDC);
			}
		}
		if (this_node->GetChild3() == nullptr)
		{
			child_node = CreateNode(this_node, DROITE, pDC); //check si une branche a droite
			child_node->FlgDirection = DROITE;

			if (GetProbability(PROBABILITE))
			{
				child_node->FlgFnDeb = DEBUT;
				SetPosAng(child_node,XXX,YYY,pDC);
				Dessine(child_node,XXX,YYY, pDC);
			}
		}
		if (this_node->GetChild2() == nullptr)
		{
			child_node = CreateNode(this_node, PRINCIPAL, pDC);
			child_node->FlgDirection = PRINCIPAL;
			child_node->FlgFtAvRec = FAIT;
			SetPosAng(child_node,XXX,YYY,pDC);
			Dessine(child_node,XXX,YYY, pDC);
			this_node = this_node->GetChild2(); // avance de un node
		}
	}
	if (this_node->GetChild1() == nullptr)
	{
		child_node = CreateNode(this_node, GAUCHE, pDC); // check si unebranche a gauche
		child_node->FlgDirection = GAUCHE;

		if (GetProbability(PROBABILITE))
		{
			child_node->FlgFnDeb = DEBUT;
			SetPosAng(child_node,XXX,YYY, pDC);
			Dessine(child_node,XXX,YYY, pDC);
		}
	}
	if (this_node->GetChild3() == nullptr)
	{
		child_node = CreateNode(this_node, DROITE, pDC); //check si une branche a droite
		child_node->FlgDirection = DROITE;

		if (GetProbability(PROBABILITE))
		{
			child_node->FlgFnDeb = DEBUT;
			SetPosAng(child_node,XXX,YYY, pDC);
			Dessine(child_node,XXX,YYY, pDC);
		}
	}
	
	static int cpt = 0;
	cpt++;
//	if (cpt > 1000000) return false;
//	this_node->SetEpaisseur(7);
	GoRevientNode(this_node/*->GetParent()*/,XXX,YYY, pDC);
	return false;
}
void SetUpParm(node* p,CDC* pDC)
{

	node* PPar = p->GetParent();
	if (PPar != nullptr) p->SetAngleCourante(PPar->GetAngleCourante() + 0);
			else p->SetAngleCourante(p->GetAngleCourante() + 0);
	if (PPar != nullptr) p->SetEpaisseur(PPar->GetEpaisseur() / 2);
			else p->SetEpaisseur(12);
//	MyOut(400, 100, CString("angle (%d)"), PPar->GetAngleCourante(), pDC);
//
//	MyOut(CString("epais (%d)"), PPar->GetEpaisseur(), pDC);
	return;

}
bool GoRevientNode(node* e_node,int XXX,int YYY, CDC* pDC)
{
	static int compt = 0;
	node* tp_node = e_node;
/**/	if (tp_node == nullptr) return true;   // arbre terminer 
	node* par_node; node* tpCh1_node; node* tpCh3_node; node* tpCh2_node;

/**/	par_node = tp_node;

/*******************************************************************************************/
	while (!((par_node->FlgDirection == PRINCIPAL) && (par_node->FlgFnDeb == DEBUT)))
	{
		static int cpt1 = 0;
		cpt1++;
//		if (cpt1 > 1000000) return false;
//		MyOut(300, 100, CString("cpt1=%d<"), cpt1, pDC);

		tpCh1_node = par_node->GetChild1();
		tpCh3_node = par_node->GetChild3();
		tpCh2_node = par_node->GetChild2();


		if ((par_node->FlgFnDeb == DEBUT) && (par_node->FlgFtAvRec == FAIT)) return true;


		if ((par_node->FlgFnDeb == DEBUT) && (par_node->FlgFtAvRec != FAIT)) par_node->FlgFtAvRec = FAIT;

		if (tpCh1_node->FlgFnDeb == DEBUT)
		{
							//		tpCh1_node->SetEpaisseur(10);
				//			SetUpParm(tpCh1_node, pDC);
							//		tpCh1_node->SetAngleCourante(35);
			GoDrawOneNode(tpCh1_node,XXX,YYY, pDC);
		}
		if (tpCh3_node->FlgFnDeb == DEBUT)
		{
						//			MyOut(CString("getminmax = %d "), GetMinMax(3,9), pDC);
						//		tpCh3_node->SetEpaisseur(10);
						//SetUpParm(tpCh3_node, pDC);
						//		tpCh3_node->SetAngleCourante(-35);
			GoDrawOneNode(tpCh3_node,XXX,YYY, pDC);

		}
		par_node = par_node->GetParent();// recule un noeud	
		if (par_node == nullptr) return 0;
/*		tpCh1_node = par_node->GetChild1();
		tpCh3_node = par_node->GetChild3();
		tpCh2_node = par_node->GetChild2();*/

	}
	/****************************************************************************************/
	return true;

}

// CProjectARBREView construction/destruction

CProjectARBREView::CProjectARBREView() noexcept
{
	// TODO: add construction code here

}

CProjectARBREView::~CProjectARBREView()
{
}

BOOL CProjectARBREView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CView::PreCreateWindow(cs);
}

// CProjectARBREView drawing

void CProjectARBREView::OnDraw(CDC* pDC)
{
	CProjectARBREDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: add draw code for native data here

	node* nbd = new node(nullptr);   //root node debut de l arbre PRINCIPAL
	srand(time(0));
	GoDrawOneNode(nbd, 350, 580, pDC);
	node* nbd2 = new node(nullptr);   //root node debut de l arbre PRINCIPAL
	GoDrawOneNode(nbd2, 350+300, 580, pDC);
	node* nbd3 = new node(nullptr);   //root node debut de l arbre PRINCIPAL
	GoDrawOneNode(nbd3, 350+600, 580, pDC);
	node* nbd4 = new node(nullptr);   //root node debut de l arbre PRINCIPAL
	GoDrawOneNode(nbd4, 350+900, 580, pDC);

	delete nbd;
	//[i];
	int i = 0;
	while (!mem.empty()) {
		delete mem.top();
		mem.pop();
	}
}


// CProjectARBREView diagnostics

#ifdef _DEBUG
void CProjectARBREView::AssertValid() const
{
	CView::AssertValid();
}

void CProjectARBREView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CProjectARBREDoc* CProjectARBREView::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CProjectARBREDoc)));
	return (CProjectARBREDoc*)m_pDocument;
}
#endif //_DEBUG


// CProjectARBREView message handlers


void CProjectARBREView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: Add your message handler code here and/or call default

	if (69 == nChar) ::PostQuitMessage(1);   // e key
	if (80 == nChar) {
		Invalidate(TRUE); UpdateWindow();
	}//PostMessage(WM_PAINT); }   // p key

	CView::OnKeyDown(nChar, nRepCnt, nFlags);
}
