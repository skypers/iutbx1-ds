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

#include <iostream> // pour tests
#include "gameplay.h"
#include "array.h"

using namespace std;

bool main_loop(SDL_Surface *ps) {
    SDL_Event event;
    bool quit = false;
    gameboard gb;
    diamond *pSelected = 0;
    int score = 0;
    int comboScore;
    int temps; 	 
	int temps_restant;
	int tps ;
	
	temps = 180;
	tps = time(0);
	
	temps_restant = temps - ( time(0) - tps);

    load_theme("themes/fractal_cosmos/", gb);
    init_gameboard(gb, 8, 8);

    while (!quit) {
        SDL_FillRect(ps, 0, SDL_MapRGB(ps->format, 255, 255, 255));
        temps_restant = temps - ( time(0) - tps);

	while (SDL_PollEvent(&event)) {
	    if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE)
                quit = true;
            if (event.type == SDL_MOUSEBUTTONUP && event.button.button == SDL_BUTTON_LEFT) {
                if (cursor_in_grid(event, gb)) {
                    if (!pSelected) {
                        pSelected = &query_diamond(gb, event.motion.x/DIAMOND_SIZE, event.motion.y/DIAMOND_SIZE);
                        pSelected->sub.y = DIAMOND_SIZE;
                    } else {
                        pSelected->sub.y = 0;
                        if ( try_swap(gb, *pSelected, query_diamond(gb, event.motion.x/DIAMOND_SIZE, event.motion.y/DIAMOND_SIZE), ps) ) {
                            comboScore = 1;
			    do {
                                score += gb.nb_expl * comboScore;
                                ++comboScore;                                

				show_gameboard(gb, ps, score, temps_restant);
                                explode(gb, ps);
				get_down(gb, ps);
			    } while ( check_explode(gb) );

                            cout << "score : " << score << endl;
			    if (!check_solution(gb)) {
				;
			    } else {
				cout << '(' << gb.index_sol%8 << ';' << gb.index_sol/8 << ") est une solution" << endl;
			    }
			}
			
			pSelected = 0;
                    }
                }
            }
	}
	
	show_gameboard(gb, ps, score, temps_restant);
        SDL_Flip(ps);
    }

    free_theme(gb);
    return true;
}

bool cursor_in_grid(SDL_Event e, gameboard const &gb) {
    return (e.motion.x >= 0 && e.motion.x <= gb.row*DIAMOND_SIZE)
        && (e.motion.y >= 0 && e.motion.y <= gb.col*DIAMOND_SIZE);
}
