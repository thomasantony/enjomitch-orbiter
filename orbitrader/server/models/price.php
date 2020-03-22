<?

class Price extends AppModel {
	var $name = 'Price';
		// defines how many levels deep will a search for Base model go. (Price -> Product)
	var $recursive = 1;
		// a given Price is used by only one product
	//var $hasOne = array('Product'=>array(
	//				'className'=>'Product'//,
	//					// foreignKey: the name of the foreign key that points to the associated model.
	//				//'foreignKey'=>'product_id'
	//			)
	//		);
	
	var $belongsTo = array('Product'=>array(
					'className'=>'Product'
				)
			);
}
