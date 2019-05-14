# run: check your environment already prepare
# venv: make you install virtual environment on python 3.6
# source: activate the vitural environment
# unbuntun: install the package before install pygraphviz package on python
# mac: like above, but on the OSX
# package: install requirement package on python
SOURCE := $(source venv/bin/activate)



run:ba.py
	python ba.py

venv:
	virtualenv --python=python3.6 venv
source:
	echo $(SOURCE)
unbuntun:
	# before use pygraph, we need to install some package
	sudo apt-get install graphviz libgraphviz-dev pkg-config
	sudo apt-get install python-pip python-virtualenv
mac:
	pip install graphviz
	pip install cgraph
	export PKG_CONFIG_PATH=/usr/local/lib/pkgconfig 
	cd /usr/local/include/graphviz 
	sudo ln -s . graphviz 
	pip install pygraphviz

packge:
	pip install -r requirement.txt

