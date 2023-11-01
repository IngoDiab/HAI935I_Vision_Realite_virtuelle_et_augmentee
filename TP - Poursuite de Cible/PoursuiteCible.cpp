/*
 *  PoursuiteCible.cpp
 *  
 *
 *  Created by Olivier Strauss on 17/10/16.
 *  Copyright 2016 LIRMM. All rights reserved.
 *
 */


#include "CImg.h"
#include "string.h"
#include "stdio.h"
#include "PseudoInverse.h"

using namespace cimg_library;

#include <iostream>
using namespace std;

#include <chrono>

#define TRANS_X 1
#define TRANS_Y 1

#define CORRELATION 1
#define FLUX_OPTIQUE 0

double* _matriceA, *_matriceU, *_motif_X, *_motif_Y, *_motif_onTarget, *_motif_onTargetX, *_motif_onTargetY, *transfo;

void Correlation(double* _motif, const CImg<unsigned char>& _imageTarget, int carre_x[2], int carre_y[2], int _threshold,  double& _delta_x, double& _delta_y)
{
	double _maxCorrelation = numeric_limits<double>::min();
	int _lengthX = carre_x[1] - carre_x[0];
	int _lengthY = carre_y[1] - carre_y[0];

	double* _motif_test = ALLOCATION(_lengthX*_lengthY,double);

	int _minXPossible = carre_x[0]-_threshold;
	int _minYPossible = carre_y[0]-_threshold;

	int _maxXPossible = carre_x[1]+_threshold;
	int _maxYPossible = carre_y[1]+_threshold;

	for(int _x = _minXPossible; _x < _maxXPossible; _x+=TRANS_X)
		for(int _y = _minYPossible; _y < _maxYPossible; _y+=TRANS_Y)
		{
			int _beginCropX = _x;
			int _beginCropY = _y;

			int _endCropX = _beginCropX+_lengthX;
			int _endCropY = _beginCropY+_lengthY;

			CImg<unsigned char> _motifOnTarget = _imageTarget;
			_motifOnTarget.crop(_beginCropX,_beginCropY,_endCropX-1,_endCropY-1);
			
			unsigned char* _motifCropped = _motifOnTarget.begin();
			for(int i = 0; i < _lengthX*_lengthY; ++i)
				_motif_test[i] = (double)_motifCropped[i];

			double _correlation = CorrelationPearson(_motif, _motif_test, _lengthX, _lengthY);
			if(_correlation <= _maxCorrelation) continue;
			_maxCorrelation = _correlation;
			_delta_x =  _x - carre_x[0];
			_delta_y =  _y - carre_y[0];
		}
}

void CreatePseudoInverseA(double* _deriveX, double* _deriveY, double* A, int _nbLines)
{
	for(int i = 0; i < _nbLines; ++i)
	{
		A[2*i] = _deriveX[i];
		A[2*i+1] = _deriveY[i];
	}

	PseudoInverse(A, _nbLines, 2);
}

int main(int argc, char *argv[])
{
	double _meanTime = 0;
	int nombre_de_points, n = 0, increment, m ;
	
	const unsigned char red[] = { 255,0,0 }, green[] = { 0,255,0 }, blue[] = { 0,0,255 };
	
	char NomFichier[1024], *pt, *ptc, *fin ;
	char NomDeBase[1024], Suffixe[32], ok ;
	FILE *fichier ;
	
	CImg<unsigned char> Image_lue ;
	CImgDisplay Image_disp, Detail_disp ;
	CImg <unsigned char>::iterator Valeur_image ; 
	CImg <unsigned char>::iterator Valeur_detail ; 
	CImg <double>::iterator pointeur ; // pointeur de recuperation des valeurs de niveau de gris de l'image
	CImg <unsigned char> Detail ; // recuperation du motif en poursuite

	int carre_x[2], carre_y[2], nx, ny, dim ;
	int Nx, Ny ;	
	double delta_x=0.0, delta_y=0.0, dx, dy ;
	int x, y ;

	double *Image, *Image_x, *Image_y, *Image_transformee, *pti, *ptm, *finm ; 
	double *Valeur_motif, *Derivee_temporelle, *temp; 
	int *Point_x, *Point_y ;
	
	if(argc<1) return 0 ;	
	
 	strcpy(NomFichier,argv[1]) ;
	printf("\nNom = %s\n\n",NomFichier);
	fin = strrchr(NomFichier,'.') ;
	strcpy(Suffixe,fin) ;
	printf("suffixe = %s\n\n",Suffixe);
	fin -- ;
	while( ((*fin)>='0') && ((*fin)<='9') && fin>NomFichier ) fin -- ;
	fin ++ ;
	printf("suffixe = %s\n\n",fin);
	
	ptc = NomDeBase ;
	pt = NomFichier ;
	while(pt<fin) (*ptc++) = (*pt++) ; (*ptc) = (char)0 ;
	printf("NomDeBase = %s\n\n",NomDeBase);
	
	ok = 1 ;
	increment = 1 ;
	
	while(ok)
	{
		// Lecture de l'image courante
		if(increment<10)
			sprintf(NomFichier, "%s00%d%s",NomDeBase, increment, Suffixe) ;
		else
			if(increment<100)
				sprintf(NomFichier, "%s0%d%s",NomDeBase, increment, Suffixe) ;
		else 
			sprintf(NomFichier, "%s%d%s",NomDeBase, increment, Suffixe) ;

		// tentative pour ouvrir le fichier (pour voir s'il existe)
		fichier = fopen(NomFichier,"r") ;
		
		ok = (fichier!=NULL) ;
		if(ok) fclose(fichier) ; 
		
		if(ok) // si le fichier existe lire l'image
		{ // et l'afficher dans une fenetre
			Image_lue.load(NomFichier) ;
			Image_disp.display(Image_lue) ; 
		}
		
		if(increment==1) // s'il s'agit de la premiere image
		{
			Nx = Image_lue.width() ;
			Ny = Image_lue.height() ;			
			
			Image = ALLOCATION(Nx*Ny,double); 
			Image_x = ALLOCATION(Nx*Ny,double); 
			Image_y = ALLOCATION(Nx*Ny,double); 		
			Image_transformee = ALLOCATION(Nx*Nx,double) ;	
		}
	
		Valeur_image = Image_lue.begin() ;
		pti = Image ;
		for(y=0 ; y<Ny ; y++)
		{
			for(x=0 ; x<Nx ; x++)
			{
				(*pti++) = (double)(*Valeur_image++) ;
			}
		}
							
		nombre_de_points = 4 ;

		if(increment==1)
		{
			// selection du motif
			
			nombre_de_points = Max(nombre_de_points,3) ;
			// au moins trois point pour dÃ©finir un carre
			while (!Image_disp.is_closed() && n<nombre_de_points)
			{
				Image_disp.set_title("%s","selectionner une zone");
				Image_disp.wait();
				if (Image_disp.button() && Image_disp.mouse_y()>=0) 
				{
					y = Image_disp.mouse_y();
					x = Image_disp.mouse_x();
					Image_lue.draw_circle(x,y,1,red).display(Image_disp);
					if(n>1)
					{
					 int k ;
						carre_x[0] = carre_x[0]<x ? carre_x[0] : x ;
						carre_x[1] = carre_x[1]>x ? carre_x[1] : x ;
						carre_y[0] = carre_y[0]<y ? carre_y[0] : y ;
						carre_y[1] = carre_y[1]>y ? carre_y[1] : y ;
					}
					else 
					{
					 carre_x[0] = x ;
					 carre_x[1] = x ;
					 carre_y[0] = y ;
					 carre_y[1] = y ;
					}
					n++ ;
				}
		 }
			
			Image_disp.set_title(" .oOo. ");

			nx = carre_x[1] - carre_x[0] ;
			ny = carre_y[1] - carre_y[0] ;
			
			dim = nx*ny ;

			if(dim == 0) return 0 ;
			
			Valeur_motif = ALLOCATION(dim, double) ;
			Derivee_temporelle = ALLOCATION(dim, double) ;
			temp  = ALLOCATION(dim, double) ;
			Point_x = ALLOCATION(dim, int) ;
			Point_y = ALLOCATION(dim, int) ;

			// Derivation de la premiere image
			Derive_image( Image, Image_x, Image_y, Nx, Ny, 0.4 ) ;

			delta_x = 0.0 ;
			delta_y = 0.0 ;
			Detail= Image_lue ; // delimitation du detail pour affichage
			Detail.crop(carre_x[0], carre_y[0], carre_x[1], carre_y[1]) ;

			
			// rechargement de l'image originale
			Image_lue.load(NomFichier) ;
			Image_disp.display(Image_lue) ;
			
			// conservation des points du motif
			// leur valeur de niveau de gris dans Valeur_motif
			// et leur coordonnees dans Point_x et Point_y
			for( y=carre_y[0], m=0 ; y<carre_y[1] ; y++)
			{
				for( x=carre_x[0] ; x<carre_x[1] ; x++, m++)
				{		
 					n = y*Nx+x ;
					Valeur_motif[m] = Image[n] ;
					Point_x[m] = x ;
					Point_y[m] = y ;
				}
			}

			cimg_library::CImg<double> _base(Valeur_motif, nx, ny, 1, 1);
					_base.save("BaseMot.bmp");
			if(FLUX_OPTIQUE)
			{
				_matriceA = ALLOCATION(dim * 2, double);
				_matriceU = ALLOCATION(2, double);
				_motif_X = ALLOCATION(dim,double);
				_motif_Y = ALLOCATION(dim,double);
				Derive_image( Valeur_motif, _motif_X, _motif_Y, nx, ny, 0.4 ) ;

				_motif_onTarget = ALLOCATION(dim,double);
				transfo = ALLOCATION(dim,double);
				_motif_onTargetX = ALLOCATION(dim,double);
				_motif_onTargetY = ALLOCATION(dim,double); 
			}

		} // fin de if increment == 1

		else 
		{
			if(CORRELATION)
			{
				double _x = 0, _y = 0;
				chrono::time_point<chrono::high_resolution_clock> _startTime = chrono::high_resolution_clock::now();
				Correlation(Valeur_motif, Image_lue, carre_x, carre_y,3, _x, _y);
				chrono::time_point<chrono::high_resolution_clock> _endTime = chrono::high_resolution_clock::now();

        		double _time = std::chrono::duration<double>(_endTime-_startTime).count();
				cout<<"Time for this image : "<<_time<<" seconds."<<endl;
				_meanTime += _time;
				carre_x[0]+=_x;
				carre_x[1]+=_x;
				carre_y[0]+=_y;
				carre_y[1]+=_y;

				Valeur_image = Image_lue.begin() ;
				Detail= Image_lue ;
				Detail.crop(carre_x[0],carre_y[0],carre_x[1],carre_y[1]) ;
				Valeur_detail = Detail.begin() ;
				pti = Valeur_motif ;
			}

			if(FLUX_OPTIQUE)
			{
				chrono::time_point<chrono::high_resolution_clock> _startTime = chrono::high_resolution_clock::now();
				//Create pseudo inverse A with current motif
				CreatePseudoInverseA(_motif_X, _motif_Y, _matriceA, dim);

				// Créez une instance CImg à partir du tableau de données
				cimg_library::CImg<double> _xmot(_motif_X, nx, ny, 1, 1);
				_xmot.save("MotifX.bmp");
				cimg_library::CImg<double> _ymot(_motif_Y, nx, ny, 1, 1);
				_ymot.save("MotifY.bmp");


				double u = 0, v = 0;
				int _nbIterations = 10;
				for(int i = 0; i<_nbIterations; ++i)
				{
					//Crop motif at same place on next image
					Detail= Image_lue ;
					for( y=carre_y[0], m=0 ; y<carre_y[1] ; y++)
						for( x=carre_x[0] ; x<carre_x[1] ; x++, m++)
						{		
							n = y*Nx+x ;
							_motif_onTarget[m] = (double)Detail[n] ;
						}
					
					Transformation(_motif_onTarget, transfo, nx, ny, u, v, 0, 1);

					//Derive temporelle entre l'image de base et actuelle (matrix B)
					Derive_temporelle_image(transfo, temp, Derivee_temporelle, nx, ny, 0.4f);

					//Get [UV]
					MatMult(_matriceA, Derivee_temporelle, _matriceU, 2, dim, 1);
					u += _matriceU[0];	
					v += _matriceU[1];
				}	
				chrono::time_point<chrono::high_resolution_clock> _endTime = chrono::high_resolution_clock::now();

        		double _time = std::chrono::duration<double>(_endTime-_startTime).count();
				cout<<"Time for this image : "<<_time<<" seconds."<<endl;
				_meanTime += _time;
				
				//Recrop as current motif
				Detail= Image_lue ;
				carre_x[0]+=u;
				carre_x[1]+=u;
				carre_y[0]+=v;
				carre_y[1]+=v;
				for( y=carre_y[0], m=0 ; y<carre_y[1] ; y++)
					for( x=carre_x[0] ; x<carre_x[1] ; x++, m++)
					{		
						n = y*Nx+x ;
						Valeur_motif[m] = (double)Detail[n] ;
					}
				Derive_image( Valeur_motif, _motif_X, _motif_Y, nx, ny, 0.4 ) ;
			}
		
			// C'est la que vous devez mettre a jour
			// la position du motif dans l'image courante
			
		}
	
			
		Detail.crop(carre_x[0], carre_y[0], carre_x[1], carre_y[1]) ;
		Detail.resize(4*nx, 4*ny) ;
		Detail_disp.display(Detail) ;
		
		Image_lue.draw_rectangle(carre_x[0],carre_y[0],carre_x[1],carre_y[1],blue,0.3).display(Image_disp);
		
	 // pour que l'image ne s'incremente que si on passe la souris sur la fenetre
		Image_disp.wait();
	 while(Image_disp.button()) ; 
	
		increment ++ ;
	}
	

cout<<"Mean time : "<<_meanTime/(double)increment<<" seconds."<<endl;

 DESALLOCATION(Image) ;
 DESALLOCATION(Image_x) ;
 DESALLOCATION(Image_y) ;
 DESALLOCATION(Image_transformee) ;
 DESALLOCATION(Valeur_motif) ;
 DESALLOCATION(Derivee_temporelle) ;
 DESALLOCATION(Point_x) ;
 DESALLOCATION(Point_y) ;
	return 0 ;

}

