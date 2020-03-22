<?php
class AppController extends Controller
{
	function beforeRender() {
		if (!$this->Session->check('User')) {
			$this->set('logged', false);
		} else { 
			$this->set('logged', true);
			$user = $this->Session->read('User');
			$this->set('username', $user['username']);
		}
	}
	
	function checkSession()
	{
		if (!$this->Session->check('User')) {
			$this->redirect('/users/login');
			exit();
		}
	}
}
?>
