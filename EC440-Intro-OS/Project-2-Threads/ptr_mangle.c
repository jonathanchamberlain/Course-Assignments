int ptr_mangle(int p)
{
	unsigned int ret;
	asm(" movl %1, %%eax;\n"
	" xorl %%gs:0x18, %%eax;\n"
	" roll $0x9, %%eax;\n"
	" movl %%eax, %0;\n"
	: "=r"(ret)
	: "r"(p)
	: "%eax"
	);
	return ret;
}

int ptr_demangle(int p)
{
	unsigned int ret;
	asm( " movl %1, %%eax;\n"
	" rorl $0x9, %%eax;\n"
	" xorl %%gs:0x18, %%eax;\n"
	" movl %%eax, %0\n;"
	: "=r"(ret)
	: "r"(p)
	: "%eax"
	);
	return ret;
}
