all: node_modules public/main.js

node_modules: src/main.ts package.json
	npm install

public/main.js: src/main.ts
	npm ci && npm run build

clean:
	npm run clean

.PHONY: all clean
