# Checklist
Developer checklist:
 - [] Make [planner item](https://teams.microsoft.com/l/entity/com.microsoft.teamspace.tab.planner/mytasks?tenantId=ca2a7f76-dbd7-4ec0-9108-6b3d524fb7c8&webUrl=https%3A%2F%2Ftasks.teams.microsoft.com%2Fteamsui%2FpersonalApp%2Falltasklists&context=%7B%22subEntityId%22%3A%22%2Fv1%2Fplan%2Fk_irSZfcGEyQKPazf4Q0XZYAHZ1C%22%7D) and link to this repo.
 - [] Fill in the repo's [custom properties](./settings/custom-properties), if applicable.
 - [] Click the cogwheel on the About panel, remove unnecessary items and give the repo a good description.
 - [] Add relevant file types to the gitignore and see below for research data considerations.
 - [] If LFS will be necessary, see below.
 - [] Give [access](./settings/access) to the relevant collaborators, including e.g. SAs.
 - [] Write the READMEs.
 - [] Delete this developer file.

# Git Workflow
Use the `develop` branch--or feature subbranches--to develop the project, then squash-merge to main when appropriate (e.g. when data collection begins). Moreover, use (a simplified version of) [Git Flow](https://www.atlassian.com/git/tutorials/comparing-workflows/gitflow-workflow).

# Integrated Project Structure
If this project has hardware and software components, store them in a logical structure, like below:

```
repo/
├── hardware/
│   ├── firmware/
│   │   └── sketch1/
│   │       └── sketch1.ino
│   ├── 3dprint/
│   └── pcb/
├── software/
│   ├── python/
│   │   └── main.py
│   └── web/
└── documentation/
```
The documentation folder can be used to keep manuals, data-sheets, etc.

# LFS
If you expect to have files larger than 50 MB, you must use [Git LFS](https://git-lfs.com/). To use it, create a folder to house the large files, and add it to .gitattributes like so: `**/lfs/** filter=lfs diff=lfs merge=lfs -text`. This will track all contents of any folder named lfs using LFS. Alternatively, manually add the files you wish to track with lfs.

# Ignore Research Data
Do not upload data files to the repo, unless necessary and GDPR-safe. If the scripts inside the repo process raw data, or produce data, create a suitable .gitignore file to prevent them from being uploaded. For example, create a data folder, add a .gitignore file to it, and add the content below to that. This will cause all other files in that directory to be ignored.

```
// Ignore everything in this directory, except this file:
*
!.gitignore
```