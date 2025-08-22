#include <stdarg.h>

#include "bake_expr.h"

expr_t expr_none(void)
{
	expr_t e;
	memset(&e, 0, sizeof(expr_t));
	return e;
}
expr_t expr_packi(uint64_t expr_count, ...)
{
	expr_t e;
	memset(&e, 0, sizeof(expr_t));
	e.type = EXPR_TYPE_PACK;
	e.alloc = EXPR_ALLOC_EXPRS;
	e.exprs = vec_alloc(sizeof(expr_t));
	uint64_t expr_index = 0;
	va_list args;
	va_start(args, expr_count);
	while (expr_index < expr_count)
	{
		expr_t expr = va_arg(args, expr_t);
		vec_push(&e.exprs, &expr);
		expr_index++;
	}
	va_end(args);
	return e;
}
expr_t expr_packv(vec_t exprs)
{
	expr_t e;
	memset(&e, 0, sizeof(expr_t));
	e.type = EXPR_TYPE_PACK;
	e.alloc = EXPR_ALLOC_EXPRS;
	e.exprs = exprs;
	return e;
}
expr_t expr_rule(expr_t pattern, expr_t depend, expr_t scope)
{
	expr_t e;
	memset(&e, 0, sizeof(expr_t));
	e.type = EXPR_TYPE_RULE;
	e.alloc = EXPR_ALLOC_EXPRS;
	e.exprs = vec_alloc(sizeof(expr_t));
	vec_push(&e.exprs, &pattern);
	vec_push(&e.exprs, &depend);
	vec_push(&e.exprs, &scope);
	return e;
}
expr_t expr_call(expr_t expr)
{
	expr_t e;
	memset(&e, 0, sizeof(expr_t));
	e.type = EXPR_TYPE_CALL;
	e.alloc = EXPR_ALLOC_EXPRS;
	e.exprs = vec_alloc(sizeof(expr_t));
	vec_push(&e.exprs, &expr);
	return e;
}
expr_t expr_var(expr_t expr)
{
	expr_t e;
	memset(&e, 0, sizeof(expr_t));
	e.type = EXPR_TYPE_VAR;
	e.alloc = EXPR_ALLOC_EXPRS;
	e.exprs = vec_alloc(sizeof(expr_t));
	vec_push(&e.exprs, &expr);
	return e;
}
expr_t expr_if(expr_t expr)
{
	expr_t e;
	memset(&e, 0, sizeof(expr_t));
	e.type = EXPR_TYPE_IF;
	e.alloc = EXPR_ALLOC_EXPRS;
	e.exprs = vec_alloc(sizeof(expr_t));
	vec_push(&e.exprs, &expr);
	return e;
}
expr_t expr_copy(expr_t left, expr_t right)
{
	expr_t e;
	memset(&e, 0, sizeof(expr_t));
	e.type = EXPR_TYPE_COPY;
	e.alloc = EXPR_ALLOC_EXPRS;
	e.exprs = vec_alloc(sizeof(expr_t));
	vec_push(&e.exprs, &left);
	vec_push(&e.exprs, &right);
	return e;
}
expr_t expr_copy_add(expr_t left, expr_t right)
{
	expr_t e;
	memset(&e, 0, sizeof(expr_t));
	e.type = EXPR_TYPE_COPY_ADD;
	e.alloc = EXPR_ALLOC_EXPRS;
	e.exprs = vec_alloc(sizeof(expr_t));
	vec_push(&e.exprs, &left);
	vec_push(&e.exprs, &right);
	return e;
}
expr_t expr_copy_if(expr_t left, expr_t right)
{
	expr_t e;
	memset(&e, 0, sizeof(expr_t));
	e.type = EXPR_TYPE_COPY_IF;
	e.alloc = EXPR_ALLOC_EXPRS;
	e.exprs = vec_alloc(sizeof(expr_t));
	vec_push(&e.exprs, &left);
	vec_push(&e.exprs, &right);
	return e;
}
expr_t expr_ident(str_t ident)
{
	expr_t e;
	memset(&e, 0, sizeof(expr_t));
	e.type = EXPR_TYPE_IDENT;
	e.alloc = EXPR_ALLOC_IDENT;
	e.ident = ident;
	return e;
}
expr_t expr_string(str_t string)
{
	expr_t e;
	memset(&e, 0, sizeof(expr_t));
	e.type = EXPR_TYPE_STRING;
	e.alloc = EXPR_ALLOC_STRING;
	e.string = string;
	return e;
}
expr_t expr_cond(expr_t expr)
{
	expr_t e;
	memset(&e, 0, sizeof(expr_t));
	e.type = EXPR_TYPE_COND;
	e.alloc = EXPR_ALLOC_EXPRS;
	e.exprs = vec_alloc(sizeof(expr_t));
	vec_push(&e.exprs, &expr);
	return e;
}
expr_t expr_comp(expr_t left, expr_t right)
{
	expr_t e;
	memset(&e, 0, sizeof(expr_t));
	e.type = EXPR_TYPE_COMP;
	e.alloc = EXPR_ALLOC_EXPRS;
	e.exprs = vec_alloc(sizeof(expr_t));
	vec_push(&e.exprs, &left);
	vec_push(&e.exprs, &right);
	return e;
}
expr_t expr_if_block(expr_t pack)
{
	expr_t e;
	memset(&e, 0, sizeof(expr_t));
	e.type = EXPR_TYPE_IF_BLOCK;
	e.alloc = EXPR_ALLOC_EXPRS;
	e.exprs = vec_alloc(sizeof(expr_t));
	vec_push(&e.exprs, &pack);
	return e;
}
expr_t expr_if_else_block(expr_t pack)
{
	expr_t e;
	memset(&e, 0, sizeof(expr_t));
	e.type = EXPR_TYPE_IF_ELSE_BLOCK;
	e.alloc = EXPR_ALLOC_EXPRS;
	e.exprs = vec_alloc(sizeof(expr_t));
	vec_push(&e.exprs, &pack);
	return e;
}
expr_t expr_mkdir(expr_t expr)
{
	expr_t e;
	memset(&e, 0, sizeof(expr_t));
	e.type = EXPR_TYPE_MKDIR;
	e.alloc = EXPR_ALLOC_EXPRS;
	e.exprs = vec_alloc(sizeof(expr_t));
	vec_push(&e.exprs, &expr);
	return e;
}
expr_t expr_mkfile(expr_t expr)
{
	expr_t e;
	memset(&e, 0, sizeof(expr_t));
	e.type = EXPR_TYPE_MKFILE;
	e.alloc = EXPR_ALLOC_EXPRS;
	e.exprs = vec_alloc(sizeof(expr_t));
	vec_push(&e.exprs, &expr);
	return e;
}
expr_t expr_rmdir(expr_t expr)
{
	expr_t e;
	memset(&e, 0, sizeof(expr_t));
	e.type = EXPR_TYPE_RMDIR;
	e.alloc = EXPR_ALLOC_EXPRS;
	e.exprs = vec_alloc(sizeof(expr_t));
	vec_push(&e.exprs, &expr);
	return e;
}
expr_t expr_rmfile(expr_t expr)
{
	expr_t e;
	memset(&e, 0, sizeof(expr_t));
	e.type = EXPR_TYPE_RMFILE;
	e.alloc = EXPR_ALLOC_EXPRS;
	e.exprs = vec_alloc(sizeof(expr_t));
	vec_push(&e.exprs, &expr);
	return e;
}
expr_t expr_printf(expr_t expr)
{
	expr_t e;
	memset(&e, 0, sizeof(expr_t));
	e.type = EXPR_TYPE_PRINTF;
	e.alloc = EXPR_ALLOC_EXPRS;
	e.exprs = vec_alloc(sizeof(expr_t));
	vec_push(&e.exprs, &expr);
	return e;
}
expr_t expr_shell(expr_t expr)
{
	expr_t e;
	memset(&e, 0, sizeof(expr_t));
	e.type = EXPR_TYPE_SHELL;
	e.alloc = EXPR_ALLOC_EXPRS;
	e.exprs = vec_alloc(sizeof(expr_t));
	vec_push(&e.exprs, &expr);
	return e;
}
void expr_build(expr_t expr)
{
	/*
	switch (expr->type)
	{
		case EXPR_TYPE_VAR:
		{
			switch (expr->type)
			{
				case EXPR_TYPE_COPY:
				{
					expr_t* dst = (expr_t*)vec_at(&expr->exprs, 0);
					expr_t* src = (expr_t*)vec_at(&expr->exprs, 1);
					switch (src->type)
					{
						case EXPR_TYPE_IDENT:
						{
							str_t* dst_var = ctx_get_var(dst->ident);
							str_t* src_var = ctx_get_var(src->ident);
							if (dst_var)
							{
								str_set(dst_var, str_buffer(src_var));
							}
							else
							{
								ctx_insert_var(dst->ident, *src_var);
							}
							break;
						}
						case EXPR_TYPE_STRING:
						{
							str_t* dst_var = ctx_get_var(dst->ident);
							if (dst_var)
							{
								str_set(dst_var, str_buffer(&src->string));
							}
							else
							{
								ctx_insert_var(dst->ident, src->string);
							}
							break;
						}
						default:
						{
							break;
						}
					}
					break;
				}
				case EXPR_TYPE_COPY_ADD:
				{
					expr_t* dst = (expr_t*)vec_at(&expr->exprs, 0);
					expr_t* src = (expr_t*)vec_at(&expr->exprs, 1);
					switch (src->type)
					{
						case EXPR_TYPE_IDENT:
						{
							str_t* dst_var = ctx_get_var(dst->ident);
							str_t* src_var = ctx_get_var(src->ident);
							if (dst_var)
							{
								str_append(dst_var, str_buffer(src_var));
							}
							else
							{
								ctx_insert_var(dst->ident, *src_var);
							}
							break;
						}
						case EXPR_TYPE_STRING:
						{
							str_t* dst_var = ctx_get_var(dst->ident);
							if (dst_var)
							{
								str_append(dst_var, str_buffer(&src->string));
							}
							else
							{
								ctx_insert_var(dst->ident, src->string);
							}
							break;
						}
						default:
						{
							break;
						}
					}
					break;
				}
				case EXPR_TYPE_COPY_IF:
				{
					expr_t* dst = (expr_t*)vec_at(&expr->exprs, 0);
					expr_t* src = (expr_t*)vec_at(&expr->exprs, 1);
					switch (src->type)
					{
						case EXPR_TYPE_STRING:
						{
							str_t* dst_var = ctx_get_var(dst->ident);
							if (dst_var)
							{
								//str_append(dst_var, str_buffer(&src->string));
							}
							else
							{
								//ctx_insert_var(dst->ident, src->string);
							}
							break;
						}
						default:
						{
							break;
						}
					}
					break;
				}
				default:
				{
					break;
				}
			}
			break;
		}
		case EXPR_TYPE_CALL:
		{
			switch (expr->type)
			{
				case EXPR_TYPE_MKDIR:
				{
					break;
				}
				case EXPR_TYPE_MKFILE:
				{
					break;
				}
				case EXPR_TYPE_RMDIR:
				{
					break;
				}
				case EXPR_TYPE_RMFILE:
				{
					break;
				}
				case EXPR_TYPE_PRINTF:
				{
					break;
				}
				case EXPR_TYPE_SHELL:
				{
					break;
				}
				default:
				{
					break;
				}
			}
			break;
		}
		default:
		{
			break;
		}
	}
	*/
}
void expr_print(expr_t expr, uint64_t indent_count, uint64_t indent_increment, uint8_t is_global, uint8_t is_first, uint8_t is_last)
{
	uint64_t indent_index = 0;
	while (indent_index < indent_count)
	{
		printf(" ");
		indent_index++;
	}
	switch (expr.type)
	{
		case EXPR_TYPE_NONE: printf("none\n"); break;
		case EXPR_TYPE_PACK: printf("pack\n"); break;
		case EXPR_TYPE_RULE: printf("rule\n"); break;
		case EXPR_TYPE_CALL: printf("call\n"); break;
		case EXPR_TYPE_VAR: printf("var\n"); break;
		case EXPR_TYPE_IF: printf("if\n"); break;
		case EXPR_TYPE_COPY: printf("copy\n"); break;
		case EXPR_TYPE_COPY_ADD: printf("copy_add\n"); break;
		case EXPR_TYPE_COPY_IF: printf("copy_if\n"); break;
		case EXPR_TYPE_IDENT: printf("ident %s\n", str_buffer(&expr.ident)); break;
		case EXPR_TYPE_STRING: printf("string %s\n", str_buffer(&expr.string)); break;
		case EXPR_TYPE_COND: printf("cond\n"); break;
		case EXPR_TYPE_COMP: printf("comp\n"); break;
		case EXPR_TYPE_IF_BLOCK: printf("if_block\n"); break;
		case EXPR_TYPE_IF_ELSE_BLOCK: printf("if_else_block\n"); break;
		case EXPR_TYPE_MKDIR: printf("mkdir\n"); break;
		case EXPR_TYPE_MKFILE: printf("mkfile\n"); break;
		case EXPR_TYPE_RMDIR: printf("rmdir\n"); break;
		case EXPR_TYPE_RMFILE: printf("rmfile\n"); break;
		case EXPR_TYPE_PRINTF: printf("printf\n"); break;
		case EXPR_TYPE_SHELL: printf("shell\n"); break;
		default: break;
	}
	uint64_t expr_index = 0;
	uint64_t expr_count = vec_count(&expr.exprs);
	while (expr_index < expr_count)
	{
		expr_t sub_expr = *(expr_t*)vec_at(&expr.exprs, expr_index);
		expr_print(sub_expr, indent_count + indent_increment, indent_increment, 0, expr_index == 0, expr_index == (expr_count - 1));
		expr_index++;
	}
	if (is_global && (is_last == 0))
	{
		printf("\n");
	}
}
void expr_free(expr_t expr)
{
	if (expr.alloc & EXPR_ALLOC_IDENT)
	{
		str_free(&expr.ident);
	}
	if (expr.alloc & EXPR_ALLOC_STRING)
	{
		str_free(&expr.string);
	}
	if (expr.alloc & EXPR_ALLOC_EXPRS)
	{
		uint64_t expr_index = 0;
		uint64_t expr_count = vec_count(&expr.exprs);
		while (expr_index < expr_count)
		{
			expr_t sub_expr = *(expr_t*)vec_at(&expr.exprs, expr_index);
			expr_free(sub_expr);
			expr_index++;
		}
		vec_free(&expr.exprs);
	}
	memset(&expr, 0, sizeof(expr_t));
}
