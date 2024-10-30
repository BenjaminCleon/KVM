#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libvirt/libvirt.h>

void print_hostname(virConnectPtr conn);
void list_domain_infos(virConnectPtr conn, char *state);
void destroy_domain_by_name(virConnectPtr conn, char * name);
void create_domain_by_name(virConnectPtr conn, char * name);
void save_domain_by_name(virConnectPtr conn, char * name);
void restore_domain_by_name(virConnectPtr conn, char * name);

int main(int argc, char *argv[])
{
	virConnectPtr conn;

	printf("Type an IP Address: \n");
	char ip[15];
	scanf("%s", ip);

	char uri[100];
	sprintf(uri, "qemu+ssh://%s/system", ip);
	conn = virConnectOpenAuth(uri, virConnectAuthPtrDefault, 0);
	if (conn == NULL) {
		fprintf(stderr, "Failed to open connection to qemu+tcp://localhost/system\n");
		return 1;
	}

	printf("Connection opened successfully\n");

	// Hostname
	print_hostname(conn);

    // Main loop
    int choice = 0;

    while(choice != 8)
    {
        puts("What do you want to do?");
        puts("1. List active domains");
        puts("2. List inactive domains");
        puts("3. Destroy a domain");
        puts("4. Create a domain");
        puts("5. Save a domain");
        puts("6. Restore a domain");
        puts("7. Hostname");
        puts("8. Exit");

        scanf("%d", &choice);
        if (choice == 5)
        {
            char name[100];
            printf("Enter the name of the domain to save: ");
            scanf("%s", name);
            save_domain_by_name(conn, name);
        }
        else if (choice == 6)
        {
            char name[100];
            printf("Enter the name of the domain to restore: ");
            scanf("%s", name);
            restore_domain_by_name(conn, name);
        }
        else if (choice == 4)
        {
            char name[100];
            printf("Enter the name of the domain to create: ");
            scanf("%s", name);
            create_domain_by_name(conn, name);
        }
        else if (choice == 3)
        {
            char name[100];
            printf("Enter the name of the domain to destroy: ");
            scanf("%s", name);
            destroy_domain_by_name(conn, name);
        }
        else if (choice == 2)
        {
            list_domain_infos(conn, "inactive");
        }
        else if (choice == 1)
        {
            list_domain_infos(conn, "active");
        }
        else if (choice == 7)
        {
            print_hostname(conn);
        }
    }



	virConnectClose(conn);
	return 0;
}


void list_domain_infos(virConnectPtr conn, char *state)
{
	virDomainInfoPtr domains_info = malloc(sizeof(virDomainInfo));

	virDomainPtr *domains;
	int num_domains;
	if (   strncmp(state,"active", strlen("active")) == 0 )
	{
		num_domains = virConnectListAllDomains(conn, &domains, VIR_CONNECT_LIST_DOMAINS_ACTIVE);
	}
	else if ( strncmp(state,"inactive", strlen("inactive")) == 0 )
	{
		num_domains = virConnectListAllDomains(conn, &domains, VIR_CONNECT_LIST_DOMAINS_INACTIVE);
	}

	for (int i = 0; i < num_domains; i++)
	{
		virDomainGetInfo(domains[i], domains_info);

		printf("id: %d, nom: %s\n", virDomainGetID(domains[i]), virDomainGetName(domains[i]));
		printf("state: %c\n"    , domains_info->state    );
		printf("maxMem: %ld\n"  , domains_info->maxMem   );
		printf("memory: %ld\n"  , domains_info->memory   );
		printf("nrVirtCpu: %d\n", domains_info->nrVirtCpu);
		printf("cpuTime: %lld\n", domains_info->cpuTime  );

		virDomainFree(domains[i]);
	}

	if (num_domains == 0) puts("No domains\n");

	free(domains);

	return;
}

void destroy_domain_by_name(virConnectPtr conn, char *name)
{
	virDomainPtr domain;
	domain = virDomainLookupByName(conn, name);
	virDomainDestroy(domain);
	virDomainFree(domain);
	return;
}

void create_domain_by_name(virConnectPtr conn, char *name)
{
	virDomainPtr domain;
	domain = virDomainLookupByName(conn,  name);
	virDomainCreate(domain);
	virDomainFree(domain);
	return;
}

void save_domain_by_name(virConnectPtr conn, char *name)
{
    virDomainPtr domain;
    domain = virDomainLookupByName(conn, name);
    char uri[100];
    sprintf(uri, "/var/lib/libvirt/qemu/save/%s", name);
    virDomainSave(domain, uri);
    virDomainFree(domain);
    return;
}

void restore_domain_by_name(virConnectPtr conn, char *name)
{
    char uri[100];
    sprintf(uri, "/var/lib/libvirt/qemu/save/%s", name);
    virDomainRestore(conn, uri);
    return;
}

void print_hostname(virConnectPtr conn)
{
	char *host;

	host = virConnectGetHostname(conn);
	fprintf(stdout, "Hostname:%s\n", host);
	free(host);

	return;
}