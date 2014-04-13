#ifndef COMMAND_PROMPT_LIB_H
#define COMMAND_PROMPT_LIB_H

enum TExecutionMode { emUNKNOWN, emTRAINING, emRECOGNITION, emESTIMATION };

int get_execution_mode(int argc, char *argv[]);
int train_language_model_by_mlf_file(int argc, char *argv[]);
int recognize_speech_by_mlf_file(int argc, char *argv[]);
int estimate_recognition_results(int argc, char *argv[]);

#endif //COMMAND_PROMPT_LIB_H
