#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libvirt/libvirt.h>

void print_hostname(virConnectPtr conn);
void list_domain_infos(virConnectPtr conn, char *state);
void destroy_all_active_domain(virConnectPtr conn);
void create_all_inactive_domain(virConnectPtr conn);

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

	// List Active VM
	printf("\nActive domains:\n");
	list_domain_infos(conn, "active");

	// List Inactive VM
	printf("\nInactive domains:\n");
	list_domain_infos(conn, "inactive");

	// Suspend all active domains
	destroy_all_active_domain(conn);
	printf("\nAll active domains\n");
	list_domain_infos(conn, "active");

	// Resume all inactive domains
	create_all_inactive_domain(conn);
	printf("\nAll inactive domains\n");
	list_domain_infos(conn, "inactive");

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

void destroy_all_active_domain(virConnectPtr conn)
{
	virDomainPtr *domains;
	int num_domains;
	num_domains = virConnectListAllDomains(conn, &domains, VIR_CONNECT_LIST_DOMAINS_ACTIVE);

	for (int i = 0; i < num_domains; i++) 
	{
		virDomainDestroy(domains[i]);
		virDomainFree(domains[i]);
	}

	free(domains);
	return;
}

void create_all_inactive_domain(virConnectPtr conn)
{
	virDomainPtr *domains;
	int num_domains;
	num_domains = virConnectListAllDomains(conn, &domains, VIR_CONNECT_LIST_DOMAINS_INACTIVE);

	for (int i = 0; i < num_domains; i++) 
	{
		virDomainCreate(domains[i]);
		virDomainFree(domains[i]);
	}

	free(domains);
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