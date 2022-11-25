/*
 * pANS stdio -- fscanf
 */
#pragma textflag 7
int paws_fscanf(FL_FILE *f, const char *fmt, ...){
	int n;
	va_list args;
	va_start(args, fmt);
	n=paws_vfscanf(f, fmt, args);
	va_end(args);
	return n;
}
