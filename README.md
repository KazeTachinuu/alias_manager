
# Alias Manager

Alias Manager is a simple command-line tool written in C for managing aliases in your shell. It supports adding, listing and removing aliases in your shell

## Features

- Add new aliases with custom commands
- List existing aliases
- Remove aliases by name


## Installation


1. Clone this repository to your local machine:


```shell
git clone https://github.com/kazetachinuu/alias_manager.git
cd alias_manager
```

2. Install the Alias Manager binary by running the install script:

```shell
./install.sh
```



## Usage

Alias Manager provides several subcommands for managing aliases. Here's a summary of the available subcommands:



```sh
am [SUBCOMMAND] [ARGS]
```


### Subcommands:

- `add`: Add a new alias with a custom command.


```sh
am add <ALIAS_NAME> <COMMAND>
```


- `ls`: List all existing aliases with optionnal matching string.


```sh
am ls [STRING]
```

- `rm`: Remove an alias by name.

```sh
am rm  <ALIAS_NAME> [-f|--force]
```
### Example

```sh
$ am add gp "git push"
$ am ls git

Aliases matching "git":
gca='git commit -a -m'
gs='git status'
gp='git push'
gpl='git pull'
gpt='git push --follow-tags'
```
### Options:

- `-V, --version`: Prints version information
- `-h, --help`: Prints help information for the subcommand.



Note: The alias names are case-sensitive, so make sure to provide the correct case when using alias names.


## Contributing

Contributions are welcome! If you would like to contribute to the Alias Manager project, please open an issue or submit a pull request on GitHub.

## License

This project is licensed under the [MIT License](LICENSE).

