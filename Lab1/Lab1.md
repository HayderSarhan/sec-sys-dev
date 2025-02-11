# Assignment 1

## GitLab Server

### Setting up a GitLab server

- Pulling the GitLab image and naming it:

    ```yml
    version: '3.7'

    services:
    gitlab:
        image: gitlab/gitlab-ce:latest
        container_name: 21BS242-gitlab
    ```

- Mapping the ports:

    ```yml
    ports:
      - "80:80" # Http requests port
      - "22:22" # SSH requests port
      - "443:443" # Https requests port
    ```

- Exposing the GitLab server:

    ```yml
    hostname: gitlab.test.local
    ```

    ```yml
    environment:
      GITLAB_OMNIBUS_CONFIG: |
        external_url "https://gitlab.test.local"
    ```

    we used `https` because we're gonna enable secure access later
  - **Resolving the `DNS` record:**

    This can be done by editing the `/etc/hosts` file

    ![hosts](/src/hosts.png)

    - **Enable HTTPS access:**

        First we generating the certificate for `gitlab.test.local`
        ![certificate](/src/gitlab-cert.png)
        This will generate two files holding the certificate and the privet key \
        Now all we have to do is copy these files to the `GITLAB_HOME` that we are using in our docker compose file.

        Now we configure the environment to use SSL and use the files we generated:

        ```yml
        environment:
            GITLAB_OMNIBUS_CONFIG: |
                external_url "https://gitlab.test.local"

                nginx['ssl_certificate'] = "/etc/gitlab/ssl/gitlab.test.local.crt"
                nginx['ssl_certificate_key'] = "/etc/gitlab/ssl/gitlab.test.local.key"
        ```

- Binding the necessary directories:

    ```yml
        volumes:
      - '$GITLAB_HOME/config:/etc/gitlab'
      - '$GITLAB_HOME/data:/var/opt/gitlab'
      - '$GITLAB_HOME/logs:/var/log/gitlab'
      - '$GITLAB_HOME/ssl:/etc/gitlab/ssl'
    ```

    The `$GITLAB_HOME` is an environment variable set on the loacl machine

    ```bash
    export GITLAB_HOME=/srv/gitlab
    ```

- Disabling unneeded services:

    This can be done by configuring the environment to disable the unwanted services:

    ```yml
    environment:
      GITLAB_OMNIBUS_CONFIG: |

        ...

        registry['enable'] = false
        mattermost['enable'] = false
        gitlab_pages['enable'] = false
        gitlab_kas['enable'] = false
    ```

    References:
  - [Pre-configure Docker container](https://docs.gitlab.com/ee/install/docker/configuration.html#pre-configure-docker-container)
    - [Linux package template](https://gitlab.com/gitlab-org/omnibus-gitlab/blob/master/files/gitlab-config-template/gitlab.rb.template)

### Running the docker-compose file

We run the command `docker-compose up -d`:

![compose-up](/src/compose-up.png)

Then we check if the image is running by running the command `docker ps`:
![run-check](/src/check.png)

And now we can open GitLab by visiting `https://gitlab.test.local` on the browser:
![gitlab-login](/src/gitlab-login.png)

### Creating an empty repository

In order to be able to create a new repository we first have to sign in.\
We can sign in as the root user and to get the password we run the command:

```bash
docker exec -it [image-name] grep 'Password:' /etc/gitlab/initial_root_password
```

![login](/src/login.png)

Now we have access to the root account and we can create a new project
![main-page](/src/main-page.png)
![new-repo](/src/new-repo.png)

Reference: [Install GitLab in a Docker container](https://docs.gitlab.com/ee/install/docker/installation.html#install-gitlab-by-using-docker-engine)

## GitLab Runner

### Setting up the GitLab runner

- **GitLab Runner Executor:** gitlab runner executor is the way gitlab runner works to execute jobs in a pipeline

- **Downloading the GitLab Runner:** We get the runner by following the installation steps on their page: [Install GitLab Runner](https://docs.gitlab.com/runner/install/)

- **Registering the GitLab Runner:**
  - We first create a new runner instance on GitLab, making sure to give it the appropriate name tag
    - We follow the steps on the page to register it, setting the executor to `shell`:

        ![register](/src/runner-register.png)

    - We run the gitlab runner:

        ![run](/src/run-runner.png)

        ![success](/src/successful-runner.png)

## GitLab SAST

### Choosing a vulnerable app

I decided to work with [DVWA](https://github.com/digininja/DVWA) and cloned the repository by running:

```bash
git clone https://github.com/digininja/DVWA.git
```

### Removing the `.git` file and pushing the repository to the local GitLab server

- I deleted the `.git` file as follow:

    ![delete-git](/src/remove-git.png)

- Pushing the repository:\
    I first moved the content of the DVWA to my working directory and then pushed the changes to the GitLab server, following the instructions provided by GitLab

### Adding the `.gitlab-ci.yml` file

After removing the `.github` folder form the repository, I created the `.gitlab-ci.yml` to start working on the CI

```yml
stages:
- scan # Define the scaning stage

semgrep: # Start a job called semgrep inside the scan stage
    stage: scan
    script:
        - semgrep ci --json-output=findings.json # Run the semgrep scan and save the findings into a json fiile
    artifacts:
        paths:
        - findings.json # Save the findings generated by the scan as an artifact
```

### Testing the CI

After pushing the changes to the main branch, the CI created started working and it passed, generating a report as an artifact
    ![semgrep-scan](/src/semgrep-scan.png)
    ![artifact](/src/artifact.png)

### Scan report and findings

- We can check the findings by downloading the generated artifact and checking it, or by checking our semgrep account where we will find the findings there

    ![scan-results](/src/scan-results.png)

- **Findings:**\
    [CWE-78: OS Command Injection](https://cwe.mitre.org/data/definitions/78.html):\
     This vulnerability occurs when an application constructs an OS command using user-supplied input and fails to properly neutralize special characters. This can allow attackers to inject arbitrary commands into the system, leading to code execution, system compromise, or privilege escalation.

     How to mitigate it:

  - **Validate Input:**
    Only accept expected input (e.g., IP addresses) using validation functions like `filter_var()` for IP validation.
    - **Escape Input:**
    Use `escapeshellarg()` and `escapeshellcmd()` to properly escape user input before passing it to system commands.
    - **Avoid OS Commands:**
    Whenever possible, avoid using system commands. Use safer, built-in methods in the programming language (e.g., network functions instead of ping).
    - **Least Privilege:**
    Run commands with the lowest possible privileges to minimize damage if exploited.
