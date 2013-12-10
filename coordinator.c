#include <mpi.h>
#include <stdio.h>
#include <unistd.h>
#include <SDL/SDL.h>
#include <SDL/SDL_ttf.h>

#define NBR_CASE 12 
#define WIDTH 400
#define HEIGHT 300
#define NBR_LINE 3
#define NBR_COL  4


SDL_Color cWhite = {255, 255, 255}, cBlack = {0,0,0};
SDL_Surface* lSurface[NBR_CASE] = {NULL};
SDL_Rect     lPosition[NBR_CASE];
SDL_Surface* ecran;
TTF_Font *police = NULL;
Uint32         lColorTemp[255] = {0};

void init() {

    if(SDL_Init(SDL_INIT_VIDEO) == -1) {
        printf("Error ! %s", SDL_GetError());
        exit(EXIT_FAILURE);
    }
    
    if(TTF_Init() == -1) {
        printf("Error FONT ! %s", TTF_GetError());
        exit(EXIT_FAILURE);
    }
    
    SDL_WM_SetCaption("Plaque Chauffante", NULL);
    ecran = SDL_SetVideoMode(WIDTH, HEIGHT, 32, SDL_HWSURFACE | SDL_RESIZABLE | SDL_DOUBLEBUF);    
}

void initGrille() {
    int i = 0;
    int j = 0, k = 0; // j hauteur, k largeur
    
    for(i = 0; i < 12; i++) {
            lSurface[i] = SDL_CreateRGBSurface(SDL_HWSURFACE, WIDTH/NBR_COL, HEIGHT/NBR_LINE, 32, 0, 0, 0, 0);
            lPosition[i].w = WIDTH/NBR_COL;
            lPosition[i].h = HEIGHT/NBR_LINE;
            
            if(lPosition[i].w * k >= WIDTH) {
                k = 0;
                j++;
            }
            lPosition[i].x = (lPosition[i].w * k);
            lPosition[i].y = (lPosition[i].h * j);
            
            k++;
    }
}

int initColor() {
    int i = 0;
    int blue = 254;
    int red  = 0;
    int green = 254;
    
    for(i = 0; i < 255; i++) {
        

        if(i > 255/2) {
            red = 255;
            green -= 2;
            
        } else if(i < 255/2) {
            blue -= 2;
            red += 2;
        }
        
        if(green < 0)
            green = 0;
        if(green > 255)
            green = 255;
        
        if(red < 0)
            red = 0;
        if(red > 255)
            red = 255;
        
        if(blue < 0)
            blue = 0;
        if(blue > 255)
            blue = 255;
        
        lColorTemp[i] = SDL_MapRGB(ecran->format, red, green, blue);
    }
}

Uint32 generateColorFromTemp(int temp) {
    // -100 blue complet, 155 red complet
    int val = temp + 100;
    
    if(val < 0)
        val = 0;
    if(val > 254)
        val = 254;

    return lColorTemp[val];
}


void display_tmp(double tempValues[12], int pass) {
	int i;
        char c[256];
        SDL_Rect position;
        SDL_Rect pos;
        SDL_Surface *text = NULL;
	for (i=0;i<12;i++){
		printf("Coordinator : Slave %d  temp = %f \n",i+1,tempValues[i]);
	}
        
        sprintf(c, "Plaque Chauffante - Passe %i", pass);  
        SDL_WM_SetCaption(c, NULL);
        
        // bkg
        SDL_FillRect(ecran, NULL, SDL_MapRGB(ecran->format, 17, 206, 112)); 
        
        //paint grille
        for(i = 0; i < NBR_CASE; i++) {
            sprintf(c, "%0.0f °C", tempValues[i]);
            
            SDL_FillRect(lSurface[i], NULL, generateColorFromTemp(tempValues[i]));
            text = TTF_RenderText_Blended(police, c, cBlack);
            
            pos = lPosition[i];
            pos.x += (((WIDTH/NBR_COL)/2) - (text->w/2));
            pos.y += (((HEIGHT/NBR_LINE)/2) - (text->h/2));
            
         
            SDL_BlitSurface(lSurface[i], NULL, ecran, &lPosition[i]);
            SDL_BlitSurface(text, NULL, ecran, &pos);
            
            sprintf(c, "%i", i);
            text = TTF_RenderText_Blended(police, c, cBlack);
            pos = lPosition[i];
            pos.x += 5;
            pos.y += 5;
            
            SDL_BlitSurface(text, NULL, ecran, &pos);
        }

        
       SDL_Flip(ecran); 
}


int main( int argc, char *argv[] ) {
	int signal, myrank,i,j, width;
	double ambiant;
	double temperature;
	MPI_Comm parent;
	MPI_Status etat;
        int ii = 0;
         
	MPI_Init (&argc, &argv);
	MPI_Comm_get_parent (&parent);
	MPI_Comm_rank (MPI_COMM_WORLD,&myrank);
	double tempValues[12];
        
        
        // init DISPLAY
       
        init();
        initColor();
        initGrille();
        police = TTF_OpenFont("OptimusPrinceps.ttf", 15);
        // end
        
        
        width = 4;

	if (parent == MPI_COMM_NULL) {
		printf ("Coordinator %d : Coordinator : No master :( !\n", myrank);
	} else {
		// INIT
		MPI_Recv(&ambiant, 1, MPI_DOUBLE, 0, 0, parent, &etat);
                MPI_Send(&signal, 1, MPI_INT, 0, 0, parent);
		// SIMULATION
		MPI_Recv(&signal, 1, MPI_INT, 0, 0, parent, &etat); // Go simu from master
		for (j=0;j<10;j++) {
                    
			for (i=0; i<12;i++) {
                                
				MPI_Send(&ambiant, 1, MPI_DOUBLE, i, 0,MPI_COMM_WORLD);
                                MPI_Send(&width, 1, MPI_INT, i, 0 , MPI_COMM_WORLD);	
                                MPI_Send(&signal, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
			}
                        
                        for(i = 0; i < 12; i++) {
                                
                                MPI_Recv(&temperature, 1, MPI_DOUBLE, i,0,MPI_COMM_WORLD, &etat);
				tempValues[i] = temperature;	
                                //printf("[Coordinator] Temperature Received %0.0f !\n", temperature);
                        }
                        display_tmp(tempValues, j);
                        sleep(1);
			printf("== Attente de la nouvelle Passe ==\n");
		}
                
		MPI_Send(&signal,1,MPI_INT,0,0,parent); // end of simulation sended to master
 	}
	MPI_Finalize(); 
        
        // close display
        TTF_CloseFont(police);
        TTF_Quit();
        SDL_Quit();
        //end
	return 0; 
 }
