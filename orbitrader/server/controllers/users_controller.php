<?php
class UsersController extends AppController
{
	function login()
	{
		$this->set('error', false);

		if (!empty($this->data))
		{
			$someone = $this->User->findByUsername($this->data['User']['username']);

			if( !empty($someone['User']['password']) && $someone['User']['password'] == md5($this->data['User']['password']) )
			{
				$this->Session->write('User', $someone['User']);
				$this->redirect('/payloads');
			} else {
				$this->set('error', true);
			}
		}
	}

	function logout()
	{
		$this->Session->delete('User');
		$this->redirect('/');
	}
}
?>
