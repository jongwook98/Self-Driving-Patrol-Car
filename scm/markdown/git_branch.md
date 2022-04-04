## How to create a new branch using the git commands

1. Download the latest source,
	```bash
	$ git clone git@gitlab.com:MAZE-dankook/self-driving-patrol-car.git
	or
	$ git pull origin master
	```

2. Make the new branch,
	```bash
	$ git branch <new branch name>
	```
	* If you need to check the branch lists,
	```bash
	$ git branch
	```
	* If you want to delete the branch,
	```bash
	$ git branch -d <delete branch name>
	```

3. Change the current environment to the new branch,
	```bash
	$ git checkout <new branch name>
	```

4. How to commit and push to the gitlab server,
	```bash
	$ git add <new files>
	$ git commit -s
	```
	* After that, write the commit message.
	* Please refer to the 'git log' on how to write the commit message.
	```bash
	$ git push origin <new branch name>
	```
5. Merge request to the maintainer(To Jerry),
	* On the gitlab, select the 'Merge Requests' -> 'New Merge Request'
	* After that, select the your source branch and click the 'Compare branches and continue' button.
	![Git_README_1](https://user-images.githubusercontent.com/54479819/72438906-37b8ff80-37e9-11ea-9b48-1059b0f416af.png)
	* Finally, Input each blank such as "Title", "Description" about the commit, select the "Assignee" to you, check the "Milestone", "Lavels" and click the "Submit merge request".
	![Git_README_2](https://user-images.githubusercontent.com/54479819/72438924-40113a80-37e9-11ea-99b4-9a0b8ce95ea8.png)

## How to pull the special branch

* If you want to pull the special branch, execute the commands below,
```bash
$ mkdir -p <for the branch folder>
$ cd <branch folder>
$ git clone -b <branch_name> --single-branch git@gitlab.com:MAZE-dankook/self-driving-patrol-car.git
```
