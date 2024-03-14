all: node_modules public/main.js

node_modules:
	npm install

public/main.js:
	npm ci && npm run build

clean:
	npm run clean

.PHONY: all clean
