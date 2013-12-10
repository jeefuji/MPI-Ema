/* 
 * File:   test.c
 * Author: misterweb
 *
 * Created on 3 décembre 2013, 01:14
 */

#include <stdio.h>
#include <stdlib.h>
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
Uint32         lColorTemp[255] = {0};
int           lTemp[NBR_CASE] = {-100, -75, -50, -25, 0, 25, 50, 75, 100, 125, 150, 175};

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
            
            printf("Calc x : %i, y : %i", lPosition[i].x, lPosition[i].y);
            
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

void go() {
    int i = 0;
    // bkg
    SDL_FillRect(ecran, NULL, SDL_MapRGB(ecran->format, 17, 206, 112)); 
    
    //paint grille
    for(i = 0; i < NBR_CASE; i++) {
        SDL_FillRect(lSurface[i], NULL, generateColorFromTemp(lTemp[i]));
        SDL_BlitSurface(lSurface[i], NULL, ecran, &lPosition[i]);
    }
}

int main(int argc, char** argv) {
    int continuer = 1;
    SDL_Surface *text = NULL;
    TTF_Font *police = NULL;
    SDL_Rect position;
    
    SDL_Event event;
 
    init();
    initColor();
    initGrille();
    police = TTF_OpenFont("Amplify.ttf", 65);
    
    while(continuer) {
        
        SDL_WaitEvent(&event);
        switch(event.type)
        {
            case SDL_QUIT:
                continuer = 0;
        }
        
        go();

        text = TTF_RenderText_Blended(police, "Plaque", cWhite);

        position.x = 5;
        position.y = 5;
        SDL_BlitSurface(text, NULL, ecran, &position);
        SDL_Flip(ecran); 
    }
   
    
    
    TTF_CloseFont(police);
    TTF_Quit();
    SDL_Quit();
    return (EXIT_SUCCESS);
}

