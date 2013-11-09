#include "log.h"


struct logger init_logger(char *LOGFILE, int log_lev)
{
	static struct logger log={.level=LOG_LEV_UNDEF, .file=NULL};
	
	if(log.file!=NULL||log.level!=LOG_LEV_UNDEF||log_lev==LOG_LEV_UNDEF) return log;

	log.level=log_lev;

	if((log.file = fopen(LOGFILE, "a")) == NULL){
		log_write("%s\tCould not open logfile. Error reporting to stderr only\n", LOG_WARN);
		return log;
	}
	
	sighndlr_add(close_log, &log);
	
	return log;
}

int logprint(struct logger log,char *message, int error_level, int print_stderr)
{	
	char *Error_level[]=  \
		{ "[  DEBUG  ]", \
		"[  INFO   ]", \
		"[ WARNING ]", \
		"[  ERROR  ]", \
		"[  FATAL  ]" };


	if(error_level<log.level) return 1;

	struct tm *ti;
	time_t time_s;
	char t_buf[128];

	time(&time_s);
	ti = localtime(&time_s);

	strftime(t_buf, 128, "%d. %b %Y %H:%M:%S", ti);

	if(print_stderr)
	{
		fprintf(stderr, "%s\t%s\t%s\n", Error_level[error_level], t_buf, message);
	}

	if(log.file!=NULL)
	{
		fprintf(log.file, "%s\t%s\t%s\n", Error_level[error_level], t_buf, message);
	}
		
	return 0;
}

void *close_log(void *a)
{
	struct logger *log=a;

	fclose(log->file);
	log->file=NULL;

	return NULL;
}