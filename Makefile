# make              # to compile files and create the executables
# make pgm          # to download example images to the pgm/ dir
# make setup        # to setup the test files in test/ dir
# make tests        # to run basic tests
# make clean        # to cleanup object files and executables
# make cleanobj     # to cleanup object files only

CFLAGS = -Wall -O2 -g

PROGS = imageTool imageTest

TESTS = test1 test2 test3 test4 test5 test6 test7 test8 test9

tests_ImageLocateSubImage = test_paste1_1 test_ImageLocateSubImage1_1 test_paste1_2 test_ImageLocateSubImage1_2 test_paste1_3 test_ImageLocateSubImage1_3 test_paste2_1 test_ImageLocateSubImage2_1 test_paste2_2 test_ImageLocateSubImage2_2 test_paste2_3 test_ImageLocateSubImage2_3 test_paste3_1 test_ImageLocateSubImage3_1 test_paste3_2 test_ImageLocateSubImage3_2 test_paste3_3 test_ImageLocateSubImage3_3

tests_ImageBlur = test_ImageBlur1_1 test_ImageBlur1_2 test_ImageBlur1_3 test_ImageBlur2_1 test_ImageBlur2_2 test_ImageBlur2_3

# Default rule: make all programs
all: $(PROGS)

imageTest: imageTest.o image8bit.o instrumentation.o error.o

imageTest.o: image8bit.h instrumentation.h

imageTool: imageTool.o image8bit.o instrumentation.o error.o

imageTool.o: image8bit.h instrumentation.h

# Rule to make any .o file dependent upon corresponding .h file
%.o: %.h

pgm:
	wget -O- https://sweet.ua.pt/jmr/aed/pgm.tgz | tar xzf -

.PHONY: setup
setup: test/

test/:
	wget -O- https://sweet.ua.pt/jmr/aed/test.tgz | tar xzf -
	@#mkdir -p $@
	@#curl -s -o test/aed-trab1-test.zip https://sweet.ua.pt/mario.antunes/aed/test/aed-trab1-test.zip
	@#unzip -q -o test/aed-trab1-test.zip -d test/

test1: $(PROGS) setup
	./imageTool test/original.pgm neg save neg.pgm
	cmp neg.pgm test/neg.pgm

test2: $(PROGS) setup
	./imageTool test/original.pgm thr 128 save thr.pgm
	cmp thr.pgm test/thr.pgm

test3: $(PROGS) setup
	./imageTool test/original.pgm bri .33 save bri.pgm
	cmp bri.pgm test/bri.pgm

test4: $(PROGS) setup
	./imageTool test/original.pgm rotate save rotate.pgm
	cmp rotate.pgm test/rotate.pgm

test5: $(PROGS) setup
	./imageTool test/original.pgm mirror save mirror.pgm
	cmp mirror.pgm test/mirror.pgm

test6: $(PROGS) setup
	./imageTool test/original.pgm crop 100,100,100,100 save crop.pgm
	cmp crop.pgm test/crop.pgm

test7: $(PROGS) setup
	./imageTool test/small.pgm test/original.pgm paste 100,100 save paste.pgm
	cmp paste.pgm test/paste.pgm

test8: $(PROGS) setup
	./imageTool test/small.pgm test/original.pgm blend 100,100,.33 save blend.pgm
	cmp blend.pgm test/blend.pgm

test9: $(PROGS) setup
	./imageTool test/original.pgm blur 7,7 save blur.pgm
	cmp blur.pgm test/blur.pgm

#--------------------------------------------------------------------

test_paste1_1: $(PROGS) setup
	./imageTool pgm/small/bird_256x256.pgm pgm/small/art4_300x300.pgm paste 0,0 save tests_ImageLocateSubImage/paste1_1.pgm

test_ImageLocateSubImage1_1: $(PROGS) setup
	./imageTool pgm/small/bird_256x256.pgm tests_ImageLocateSubImage/paste1_1.pgm locate save tests_ImageLocateSubImage/locate1_1.pgm

test_paste1_2: $(PROGS) setup
	./imageTool pgm/small/bird_256x256.pgm pgm/small/art4_300x300.pgm paste 21,21 save tests_ImageLocateSubImage/paste1_2.pgm

test_ImageLocateSubImage1_2: $(PROGS) setup
	./imageTool pgm/small/bird_256x256.pgm tests_ImageLocateSubImage/paste1_2.pgm locate save tests_ImageLocateSubImage/locate1_2.pgm  

test_paste1_3: $(PROGS) setup
	./imageTool pgm/small/bird_256x256.pgm pgm/small/art4_300x300.pgm paste 43,43 save tests_ImageLocateSubImage/paste1_3.pgm

test_ImageLocateSubImage1_3: $(PROGS) setup
	./imageTool pgm/small/bird_256x256.pgm tests_ImageLocateSubImage/paste1_3.pgm locate save tests_ImageLocateSubImage/locate1_3.pgm 

test_paste1_4: $(PROGS) setup
	./imageTool extra_tests/fff_1.pgm extra_tests/fff.pgm paste 255,255 save tests_ImageLocateSubImage/paste1_4.pgm

test_ImageLocateSubImage1_4: $(PROGS) setup
	./imageTool extra_tests/fff_1.pgm tests_ImageLocateSubImage/paste1_4.pgm locate save tests_ImageLocateSubImage/locate1_4.pgm 
#--------------------------------------------------------------------

test_paste2_1: $(PROGS) setup
	./imageTool pgm/small/bird_256x256.pgm pgm/medium/mandrill_512x512.pgm paste 0,0 save tests_ImageLocateSubImage/paste2_1.pgm

test_ImageLocateSubImage2_1: $(PROGS) setup
	./imageTool pgm/small/bird_256x256.pgm tests_ImageLocateSubImage/paste2_1.pgm locate save tests_ImageLocateSubImage/locate2_1.pgm 

test_paste2_2: $(PROGS) setup
	./imageTool pgm/small/bird_256x256.pgm pgm/medium/mandrill_512x512.pgm paste 127,127 save tests_ImageLocateSubImage/paste2_2.pgm

test_ImageLocateSubImage2_2: $(PROGS) setup
	./imageTool pgm/small/bird_256x256.pgm tests_ImageLocateSubImage/paste2_2.pgm locate save tests_ImageLocateSubImage/locate2_2.pgm 

test_paste2_3: $(PROGS) setup
	./imageTool pgm/small/bird_256x256.pgm pgm/medium/mandrill_512x512.pgm paste 255,255 save tests_ImageLocateSubImage/paste2_3.pgm

test_ImageLocateSubImage2_3: $(PROGS) setup
	./imageTool pgm/small/bird_256x256.pgm tests_ImageLocateSubImage/paste2_3.pgm locate save tests_ImageLocateSubImage/locate2_3.pgm 


#--------------------------------------------------------------------

test_paste3_1: $(PROGS) setup
	./imageTool pgm/small/bird_256x256.pgm pgm/large/airfield-05_1600x1200.pgm paste 0,0 save tests_ImageLocateSubImage/paste3_1.pgm

test_ImageLocateSubImage3_1: $(PROGS) setup
	./imageTool pgm/small/bird_256x256.pgm tests_ImageLocateSubImage/paste3_1.pgm locate save tests_ImageLocateSubImage/locate3_1.pgm 

test_paste3_2: $(PROGS) setup
	./imageTool pgm/small/bird_256x256.pgm pgm/large/airfield-05_1600x1200.pgm paste 671,471 save tests_ImageLocateSubImage/paste3_2.pgm

test_ImageLocateSubImage3_2: $(PROGS) setup
	./imageTool pgm/small/bird_256x256.pgm tests_ImageLocateSubImage/paste3_2.pgm locate save tests_ImageLocateSubImage/locate3_2.pgm 

test_paste3_3: $(PROGS) setup
	./imageTool pgm/small/bird_256x256.pgm pgm/large/airfield-05_1600x1200.pgm paste 1343,943 save tests_ImageLocateSubImage/paste3_3.pgm

test_ImageLocateSubImage3_3: $(PROGS) setup
	./imageTool pgm/small/bird_256x256.pgm tests_ImageLocateSubImage/paste3_3.pgm locate save tests_ImageLocateSubImage/locate3_3.pgm 

#--------------------------------------------------------------------


test_ImageBlur1_1: $(PROGS) setup
	./imageTool pgm/small/bird_256x256.pgm blur 20,3 save tests_ImageBlur/blur1_1.pgm

test_ImageBlur1_2: $(PROGS) setup
	./imageTool pgm/small/bird_256x256.pgm blur 3,20 save tests_ImageBlur/blur1_2.pgm

test_ImageBlur1_3: $(PROGS) setup
	./imageTool pgm/small/bird_256x256.pgm blur 20,20 save tests_ImageBlur/blur1_3.pgm

#--------------------------------------------------------------------

test_ImageBlur2_1: $(PROGS) setup
	./imageTool pgm/large/einstein_940x940.pgm blur 20,3 save tests_ImageBlur/blur2_1.pgm

test_ImageBlur2_2: $(PROGS) setup
	./imageTool pgm/large/einstein_940x940.pgm blur 3,20 save tests_ImageBlur/blur2_2.pgm

test_ImageBlur2_3: $(PROGS) setup
	./imageTool pgm/large/einstein_940x940.pgm blur 20,20 save tests_ImageBlur/blur2_3.pgm

#--------------------------------------------------------------------

# extra test

test_extrapaste1_1: $(PROGS) setup
	./imageTool extra_tests/image pgm/medium/mandrill_512x512.pgm paste 511,511 save extra_tests/extra_paste1_2.pgm

test_extraTest1_1: $(PROGS) setup
	./imageTool extra_tests/image4_2.pgm extra_tests/image4_1.pgm locate save extra_tests/extra_locate4.pgm

	


.PHONY: tests
tests: $(TESTS)

.PHONY: tests_ImageLocateSubImage
tests_ImageLocateSubImage: $(tests_ImageLocateSubImage)

.PHONY: tests_ImageBlur
tests_ImageBlur: $(tests_ImageBlur)

# Make uses builtin rule to create .o from .c files.

cleanobj:
	rm -f *.o

clean: cleanobj
	rm -f $(PROGS)

