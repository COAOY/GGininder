





run:ba.py
	python ba.py

venv:
	virtualenv --python=python3.6 venv
source:
	source venv/bin/activate
ununtun:
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

