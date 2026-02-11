all: index.cpp main.cpp 
	g++ -g -o index index.cpp main.cpp html_parser.cpp html_tags.cpp

clean:
	rm -f index