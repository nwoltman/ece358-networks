# ECE 358: Computer Networks Labs

## Due Dates

**Lab 1:** June 8, 10:00 PM  
**Lab 2:** July 13, 10:00 PM  
**Lab 3:** July 28, 10:00 PM  

## Contributing

### Clone

```sh
git clone https://github.com/woollybogger/ece358-networks.git
cd ece358-networks
```

### Build & Run

Specific instructions for this will be in each lab's README.

### Commit

Since we're working in a group, it will be really helpful to have a commit message that makes it easy for everyone to understand the changes being made. Also, no one is going to be picky about this, but try to follow this format:

```
lab#: Short overview of changes

Paragraph describing the changes being made using full sentences (or one sentence is usually good enough).
You may not even need this paragraph if the short description gets the point across.
```

### Rebase

You'll need to rebase whenever someone pushes new changes to `master` on GitHub.

```sh
git fetch
git rebase origin/master
```

### Push

```sh
git push # -f if pushing after rebasing or amending a commit
```
