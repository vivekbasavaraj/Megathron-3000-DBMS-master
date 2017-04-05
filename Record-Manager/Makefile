all: assign3 test_expr clean

assign3: test_assign3_1.o expr.o dberror.o record_mgr.o rm_serializer.o storage_mgr.o buffer_mgr.o buffer_mgr_stat.o
	gcc -o test_assign3 test_assign3_1.o expr.o dberror.o record_mgr.o rm_serializer.o storage_mgr.o buffer_mgr.o buffer_mgr_stat.o -o assign3

test_assign3_1.o: test_assign3_1.c dberror.h expr.h record_mgr.h tables.h test_helper.h
	gcc -c test_assign3_1.c

record_mgr.o: record_mgr.c record_mgr.h dberror.h expr.h tables.h
	gcc -c record_mgr.c

buffer_mgr.o: buffer_mgr.c buffer_mgr.h storage_mgr.h dt.h
	gcc -c buffer_mgr.c

storage_mgr.o: storage_mgr.c storage_mgr.h dberror.h
	gcc -c storage_mgr.c

expr.o: expr.c dberror.h tables.h
	gcc -c expr.c

rm_serializer.o: rm_serializer.c dberror.h tables.h record_mgr.h
	gcc -c rm_serializer.c

buffer_mgr_stat.o: buffer_mgr_stat.c buffer_mgr_stat.h buffer_mgr.h
	gcc -c buffer_mgr_stat.c

dberror.o: dberror.c dberror.h
	gcc -c dberror.c

test_expr:  test_expr.o expr.o dberror.o record_mgr.o rm_serializer.o storage_mgr.o buffer_mgr.o buffer_mgr_stat.o
	gcc -o test_expr test_expr.o expr.o dberror.o record_mgr.o rm_serializer.o storage_mgr.o buffer_mgr.o buffer_mgr_stat.o

test_expr.o: test_expr.c dberror.h expr.h record_mgr.h tables.h
	gcc -c test_expr.c

clean:
	rm -rf *.o
