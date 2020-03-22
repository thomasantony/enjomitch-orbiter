<?php

class PayloadsController extends AppController{
	//var $scaffold;
	var $components = array('RequestXml');
	
	function index() {
		$useridtofind = 1;
		$this->set('payloads', $this->Payload->findAllByUserId($useridtofind));
	}
	
	function buy() {
		global $HTTP_RAW_POST_DATA;
		if ($this->RequestXml->isXmlRequest) {
			$xp = new DOMXPath($this->RequestXml->parsedXmlReq);
			/*
			$this->set('reqtype', 'XML');
			$this->set('request', $HTTP_RAW_POST_DATA);
			$shipname = $xp->evaluate('string(/transfer/vessel/name)');
			$this->set('parsed', "The ship you are flying is called $shipname");
			*/
			
			$userid = 1; // this will be taken from a session var
			
			$products = $xp->query('/transfer/products/product');
			foreach ($products as $product) {
				// http://manual.cakephp.org/chapter/models -> "Saving Your Data"
				
				// the DOM way 
				//$prid = (int)$product->getElementsByTagName('id')->item(0)->nodeValue;
				//$pram = (int)$product->getElementsByTagName('amount')->item(0)->nodeValue;
				
				// the shorter XPath way
				$prid = $xp->evaluate('string(id)', $product);
				$pram = $xp->evaluate('string(amount)', $product);
				
				$paylmodel = $this->Payload->findByUserIdAndProductId($userid, $prid);
				if ($paylmodel) { // if payloads of the same type are already in the bay, increase the amount
					$paylmodel['Payload']['amount'] = $paylmodel['Payload']['amount'] + $pram;
				} else {
					$this->Payload->id = null; // we don't want to modify an existing payload
					$paylmodel['Payload']['product_id'] = $prid;
					$paylmodel['Payload']['amount'] = $pram;
					$paylmodel['Payload']['user_id'] = $userid;
				}
				$this->Payload->save($paylmodel);
			}
			
			$this->set( 'bayresponse', $this->requestAction('/rest/payloads/index/', array('return')) );
				
		} else {
			$this->set('reqtype', 'normal');
			$this->set('request', '');
		}
	}
}
