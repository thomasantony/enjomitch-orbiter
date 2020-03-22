<?

class Base extends AppModel {
		// Its always good practice to include this variable.
	var $name = 'Base';
		// defines how many levels deep will a search for Base model go. (Base -> Price -> Product)
	var $recursive = 2;
		// means that Base model is assosiated with many Price models
	var $hasMany = array('Price'=>array(
					'className'=>'Price',
						// dependent: if set to true, the associated model is destroyed when this one is.
					'dependent'=>true, 
						// exclusive: If set to true, all the associated objects are deleted 
						// in one SQL statement without having their beforeDelete callback run.
					'exclusive'=>true
				)
			);
}
