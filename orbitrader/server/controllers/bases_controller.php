<?php

class BasesController extends AppController
{
	var $name = 'Bases';

		// defines index action, namely displaying all bases
	function index()
	{
			// set acts as a relator between a controller and a view
		$this->set('bases', $this->Base->findAll(null, null, null, null, null, 0));
	}
		// this function displays a base on a given planet
	function view($planet = null, $name = null) {
			// findByBaseNameAndPlanet magically expands into SQL query defined in the function's name
		$this->set('base', $this->Base->findByBaseNameAndPlanet($name, $planet));
	}
}
