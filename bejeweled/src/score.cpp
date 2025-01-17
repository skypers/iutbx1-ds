/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    Projet AP1S1 2 : Bejeweled. A bejeweled-like game written in C++ with 
    the SDL, SDL_image and SDL_ttf libraries.

    Copyright (C) 2010  Dimitri Sabadie <dimitri.sabadie@etu.u-bordeaux1.fr>
    and                 Ludwig Raepsaet <ludwig.raepsaet@etu.u-bordeaux1.fr>

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
   - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

#include <iostream>
#include <fstream>
#include <sstream>
#include "cipher.h"
#include "score.h"

TTF_Font * init_font() {
    TTF_Font *pf;
    pf = TTF_OpenFont("themes/default/NEUROPOL.ttf", 32);

    if (!pf)
	cerr << "Erreur de chargement de la police !" << endl;
    return pf;
}

void free_font(TTF_Font *pf) {
    if (pf)
	TTF_CloseFont(pf);
}

void scores(TTF_Font *f, SDL_Surface *ps, int x, int score) // nom de fonction pas du tout explicite, a modifier
{
    SDL_Rect pos;
    SDL_Surface *scoreFont;
    SDL_Color colorFont = {255,255,255,255};
    stringstream sstr;

    sstr << "Score : " << score;
    
    scoreFont = TTF_RenderText_Blended( f, sstr.str().c_str(), colorFont );
	
    if (!scoreFont)
	cerr << "Surface score non generee" << endl;
	
    pos.x = x;
    pos.y = 200;
    
    SDL_BlitSurface(scoreFont, 0, ps, &pos);
    SDL_FreeSurface(scoreFont);   
}

bool is_alphanum(Uint16 unicode) {
    return (unicode >= 'A' && unicode <= 'Z') ||
        (unicode >= 'a' && unicode <= 'z') ||
        (unicode >= '0' && unicode <= '9');
}

string get_username(int place, TTF_Font *pf, SDL_Surface *ps) {
    string nick = "___";
    stringstream message;
    unsigned int i = 0;
    SDL_Event event;
    SDL_Surface *typeArea = 0;
    SDL_Surface *congrats;
    SDL_Color white = { 255, 255, 255 };
    SDL_Color red = { 255, 0, 0 };
    bool done = false;
    SDL_Rect pos;
    SDL_Rect posCongrats;

    SDL_EnableUNICODE(1);

    message << "Bravo ! Vous etes " << place << "e !!";

    typeArea = TTF_RenderText_Blended(pf, nick.c_str(), white);
    congrats = TTF_RenderText_Blended(pf, message.str().c_str(), red);
            
    pos.x = (SCREEN_WIDTH-typeArea->w)/2;
    pos.y = (SCREEN_HEIGHT-typeArea->h)/2;
    posCongrats.x = (SCREEN_WIDTH-congrats->w)/2;
    posCongrats.y = SCREEN_HEIGHT/4 - congrats->h/2;

    SDL_FillRect(ps, 0, SDL_MapRGB(ps->format, 0, 0, 0));
    SDL_BlitSurface(congrats, 0, ps, &posCongrats);
    SDL_BlitSurface(typeArea, 0, ps, &pos);
    SDL_Flip(ps);
    SDL_FreeSurface(typeArea);

    while (!done) {
        SDL_WaitEvent(&event);

        if (event.type == SDL_KEYDOWN) {
            switch (event.key.keysym.sym) {
                case SDLK_LEFT :
                    if (i > 0)
                        --i;
                    break;

                case SDLK_RIGHT :
                    if (i < nick.size()-1)
                        ++i;
                    break;

                case SDLK_DELETE :
                    nick[i] = '_';
                    break;

                case SDLK_BACKSPACE :
                    nick[i] = '_';
                    if (i > 0)
                        --i;
                    nick[i] = '_';
                    break;

                case SDLK_RETURN :
                    done = true;
                    break;

                default :
                    if (is_alphanum(event.key.keysym.unicode)) {
                        nick[i] = event.key.keysym.unicode;
                        if (i < nick.size()-1) {
                            ++i;
                        }
                    }
            }
            
            typeArea = TTF_RenderText_Blended(pf, nick.c_str(), white);
            
            pos.x = (SCREEN_WIDTH-typeArea->w)/2;
            pos.y = (SCREEN_HEIGHT-typeArea->h)/2;

            SDL_FillRect(ps, 0, SDL_MapRGB(ps->format, 0, 0, 0));
            SDL_BlitSurface(congrats, 0, ps, &posCongrats);
            SDL_BlitSurface(typeArea, 0, ps, &pos);
            SDL_Flip(ps);
            SDL_FreeSurface(typeArea);
        }
    }

    SDL_EnableUNICODE(1);

    return nick;
}

void in_top_ten_solo(TTF_Font *pf, SDL_Surface *ps, int score) {
    fstream file;
    stringstream cdata;
    stringstream ddata;
    stringstream towrite;
    string strtmp;
    int scoretmp;

    // petit hack pour s'assurer d'ouvrir un fichier existant par la suite
    file.open(TOP_TEN_SOLO_FILE.c_str(), ios::out | ios::app);
    file.close();

    file.open(TOP_TEN_SOLO_FILE.c_str(), ios::in);

    if (file.is_open()) {
        cdata << file.rdbuf();
        file.close();

        if (cdata.str().empty()) { // si pas de scores encore references
            towrite << get_username(1, pf, ps) << ' ' << score << endl;
            for (int i = 0; i < 9; ++i)
                towrite << "___ " << 0 << endl;
        } else { // sinon, il y a deja eu des records
            ddata << decrypt(cdata.str());
        
            for (int i = 0; i < 10; ++i) {
                ddata >> strtmp >> scoretmp;
                if (score > scoretmp) {
                    towrite << get_username(i+1, pf, ps) << ' ' << score << endl;
                    score = -1;
                    ++i;
                } 
                
                if (i < 10)
                    towrite << strtmp << ' ' << scoretmp << endl;
            }
        }

        file.open(TOP_TEN_SOLO_FILE.c_str(), ios::out | ios::trunc);
        
        if (file.is_open()) {
            file << crypt(towrite.str()) << endl;
            file.close();
        } else {
            cerr << '[' << TOP_TEN_SOLO_FILE << "] fichier inaccessible en écriture" << endl;
        }
    } else {
        cerr << '[' << TOP_TEN_SOLO_FILE << "] fichier inaccessible en lecture" << endl;
    }
}
