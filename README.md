dollar
======

`dollar` is a $1, $N, $P and $P+ gesture recognition

Instructions
============

download Build system:
----------------------

	sudo pip install lutin
	sudo pip install pillow

download the software:
----------------------

	mkdir WORKING_DIRECTORY
	cd WORKING_DIRECTORY
	git clone https://github.com/atria-soft/etk.git
	git clone https://github.com/atria-soft/ejson.git
	git clone https://github.com/generic-library/gtest-lutin.git --recursive
	git clone https://github.com/generic-library/z-lutin.git --recursive

Compile software:
-----------------

	cd WORKING_DIRECTORY
	lutin dollar-test?build?run


To generate a new corpus:
-------------------------

Simply install the basic software that generate corpus with writing data (hand or stylet)

	cd WORKING_DIRECTORY
	lutin -tAndroid dollar-recorder?build?install

The corpus is register on the android board in the path /sdcard/DOLLAR/corpus

To add it at hte current corpus:

	cd WORKING_DIRECTORY/dollar/data/corpus/
	XXXXXXX/android/sdk/platform-tools/adb pull /sdcard/DOLLAR/corpus .

Use the new corpus to generate new symbols:
-------------------------------------------

a generic program is designed to do this: It have some parameter, but we keep in system the best one we have

	cd WORKING_DIRECTORY
	lutin -mdebug -cclang dollar-generate-form?build?run:--keep_ratio:appl_private/dollar/data/corpus/

The processing can take long time ...

The result are availlable on out_dollar/generate-form/corpus/

update the corpus for the applications:

	rm appl_private/dollar/tool/recorder/data/reference/*
	cp out_dollar/generate-form/corpus/* appl_private/dollar/tool/recorder/data/reference/


Bench the corpus:
-----------------

We check directly on the corpus reference

	cd WORKING_DIRECTORY
	lutin -cclang -mdebug dollar-bench-corpus?build?run:--keep_ratio:out_dollar/generate-form/corpus:appl_private/dollar/data/corpus/


Bench result auto corpus test ...
---------------------------------

Test with a keep ratio:                       81.62%
Test with a No keep ratio:                    75.63%
Test with a keep ratio <75%:                  80.80%
Automatic detect keep ratio:                  54%    ==> remove code duplication
Test keep ratio with a panality of distance:  81.92%
idem previous + check the number of points


License (APACHE v2.0)
=====================
Copyright dollar Edouard DUPIN

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.


