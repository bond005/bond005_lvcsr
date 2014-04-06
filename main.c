/* main.c
 *
 * This file is part of bond005_lvcsr.
 *
 * bond005_lvcsr is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * bond005_lvcsr is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with bond005_lvcsr. If not, see <http://www.gnu.org/licenses/>.
 *
 * Author:    Ivan Yu. Bondarenko.
 * Version:   0.1
 * Date:      2014
 * Copyright: GNU General Public License version 3.
 */

#include <stdio.h>
#include <stdlib.h>
#include "command_prompt_lib.h"

int main(int argc, char *argv[])
{
    int res = EXIT_SUCCESS;
    int mode = get_execution_mode(argc, argv);
    if (mode == emUNKNOWN)
    {
        fprintf(stderr, "There are incorrect parameters of command prompt "\
                "(execution mode of this program cannot be defined).\n");
        return EXIT_FAILURE;
    }
    if (mode == emTRAINING)
    {
        if(!train_language_model_by_mlf_file(argc, argv))
        {
            res = EXIT_FAILURE;
        }
    }
    else if (mode == emTESTING)
    {
        if(!recognize_speech_by_mlf_file(argc, argv))
        {
            res = EXIT_FAILURE;
        }
    }
    else
    {
        if (!estimate_recognition_results(argc, argv))
        {
            res = EXIT_FAILURE;
        }
    }
    return res;
}
