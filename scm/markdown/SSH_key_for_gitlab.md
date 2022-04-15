# Setup the SSH key on the Gitlab

## How to register the ssh key on the shared_codes
1. Create the ssh key through rsa
	* In Host,
	```bash
	$ mkdir -p ~/.ssh && cd ~/.ssh
	$ ssh-keygen -t rsa -f id_rsa
	```
	- And input 'enter' key
	- You can see the id_rsa.pub file in ~/.ssh folder
	```bash
	$ ls -al ~/.ssh/
	```
	- Set the ssh config file
	```bash
	$ cat << EOF > ~/.ssh/config
	>IdentityFile ~/.ssh/id_rsa
	>EOF
	$ chmod 600 ~/.ssh/config
	$ eval `ssh-agent -s`
	$ ssh-add ~/.ssh/id_rsa
	```

2. Create the git configure file
	* In Host,
	```bash
	$ cat << EOF > ~/.gitconfig
	>[user]
	>	email = <Your Email Address>
	>	name = <Your Name>
	>[core]
	>	editor = vim
	>EOF
	```

3. Register the ssh key on the gitlab
	```bash
	$ cat ~/.ssh/id_rsa.pub
	```
	- copy the contents
	- In git repository, past the contents. Please refer to the image as below
	- ![id_rsa](https://user-images.githubusercontent.com/54479819/71583951-d986ee00-2b53-11ea-99bb-95634f8f3c2e.png)

4. Pull the repository
	```bash
	$ ssh -T git@gitlab.com
	$ git clone https://gitlab.com/MAZE-dankook/self-driving-patrol-car.git
	$ cd self-driving-patrol-car/scm/scripts/common
	$ sudo pip3 install -U -r requirements.txt
	```
	- Now, You can push/pull without to input ID/Password
	- Enjoy :)
