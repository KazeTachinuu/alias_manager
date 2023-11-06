
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
aliasmanager [SUBCOMMAND] [ARGS]
```


### Subcommands:

- `add`: Add a new alias with a custom command.


```sh
aliasmanager add <ALIAS_NAME> <COMMAND>
```


- `ls`: List all existing aliases.


```sh
aliasmanager ls
```

- `rm`: Remove an alias by name.

```sh
aliasmanager rm  <ALIAS_NAME>
```
### Example

```sh
aliasmanager add -n am -c aliasmanager
```
### Options:

- `-n, --name "<ALIAS_NAME>"`: Specifies the name of the alias.
- `-c, --command <COMMAND>`: Specifies the command associated with the alias.
- `-h, --help`: Prints help information for the subcommand.



Note: The alias names are case-sensitive, so make sure to provide the correct case when using alias names.


## Contributing

Contributions are welcome! If you would like to contribute to the Alias Manager project, please open an issue or submit a pull request on GitHub.

## License

This project is licensed under the [MIT License](LICENSE).

## Acknowledgements

- [Clap](https://github.com/clap-rs/clap) - A powerful and flexible command-line argument parsing library for Rust.
- [colored](https://github.com/mackwic/colored) - A library for adding color to terminal output in Rust.
