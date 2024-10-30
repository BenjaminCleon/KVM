#include <stdio.h>
#include <stdlib.h>
#include <libvirt/libvirt.h>

void print_hostname(virConnectPtr conn);
void list_domain_infos(virConnectPtr conn);

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

    // List

    // List Active VM
    printf("Active domains:\n");
    list_domain_infos(conn, "active");

    // List Inactive VM
    printf("Inactive domains:\n");
    list_domain_infos(conn, "inactive");

	virConnectClose(conn);
	return 0;
}

void list_domain_infos(virConnectPtr conn, char *state)
{
    virDomainInfoPtr *domains_info;

    virDomainPtr *domains;
    int num_domains;
    if ( state == "active")
    {
        num_domains = virConnectListAllDomains(conn, &domains, VIR_CONNECT_LIST_DOMAINS_ACTIVE);
    }
    else if (state == "inactive")
    {
        num_domains = virConnectListAllDomains(conn, &domains, VIR_CONNECT_LIST_DOMAINS_INACTIVE);
    }

    for (int i = 0; i < num_domains; i++) 
    {
        virDomainGetInfo(domains[i], domains_info);

        printf("id: %d, nom: %s\n", virDomainGetID(domains[i]), virDomainGetName(domains[i]));
        printf("state: %c\n", domains_info->state);
        printf("maxMem: %ld\n", domains_info->maxMem);
        printf("memory: %ld\n", domains_info->memory);
        printf("nrVirtCpu: %d\n", domains_info->nrVirtCpu);
        printf("cpuTime: %ld\n", domains_info->cpuTime);   
        virDomainFree(domains[i]);
    }

    free(domains);
    free(domains_info);
}

void print_hostname(virConnectPtr conn)
{
    char *host;

    host = virConnectGetHostname(conn);
	fprintf(stdout, "Hostname:%s\n", host);
	free(host);
}