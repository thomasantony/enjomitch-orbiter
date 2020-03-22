<?php
class Payload extends AppModel {
	var $name = 'Payload';
	var $recursive = 2;
	var $belongsTo = array('Product'=>array(
					'className'=>'Product',
				)
			);
}
