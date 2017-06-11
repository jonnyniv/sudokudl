all:
	@$(MAKE) -C src
	ln -s src/sudokudlx sudokudlx

clean:
	@$(MAKE) clean -C src
	rm sudokudlx
