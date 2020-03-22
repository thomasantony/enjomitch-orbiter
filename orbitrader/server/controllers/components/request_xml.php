<?php

class RequestXmlComponent extends Object {
	var $controller = true;
	
	var $isXmlRequest;
	var $parsedXmlReq;
	
	function startup(&$controller) {
		global $HTTP_RAW_POST_DATA;
		if ($_SERVER['REQUEST_METHOD'] == 'POST' || $_SERVER['REQUEST_METHOD'] == 'PUT') {
			if ($_SERVER['CONTENT_TYPE'] == 'text/xml' || $_SERVER['CONTENT_TYPE'] == 'application/xml') {
				$this->isXmlRequest = true;
				$this->parsedXmlReq = new DOMDocument();
				$this->parsedXmlReq->loadXML($HTTP_RAW_POST_DATA);
			}
		}
	}
}

