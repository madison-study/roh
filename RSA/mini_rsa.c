#include <stdio.h>
#include <stdlib.h>
#include <openssl/rsa.h>
#include <openssl/pem.h>
#include <openssl/bn.h>

int main()
{
    int bits = 2048;
    unsigned long e_value = 65537;

    RSA *rsa = NULL;
    BIGNUM *e = NULL;
    FILE *private_file = NULL;
    FILE *public_file = NULL;

    /* RSA 구조 생성 */
    rsa = RSA_new();
    e = BN_new();

    if (!BN_set_word(e, e_value))
    {
        printf("Error setting exponent\n");
        return 1;
    }

    /* RSA 키 생성 */
    if (!RSA_generate_key_ex(rsa, bits, e, NULL))
    {
        printf("Error generating RSA key\n");
        return 1;
    }

    /* private key 저장 */
    private_file = fopen("id_rsa", "wb");
    if (!private_file)
    {
        printf("Error opening private key file\n");
        return 1;
    }

    PEM_write_RSAPrivateKey(
        private_file,
        rsa,
        NULL,
        NULL,
        0,
        NULL,
        NULL);

    fclose(private_file);

    /* public key 저장 */
    public_file = fopen("id_rsa.pub", "wb");
    if (!public_file)
    {
        printf("Error opening public key file\n");
        return 1;
    }

    PEM_write_RSA_PUBKEY(public_file, rsa);

    fclose(public_file);

    printf("RSA key pair generated\n");
    printf("Private key: id_rsa\n");
    printf("Public key : id_rsa.pub\n");

    RSA_free(rsa);
    BN_free(e);

    return 0;
}