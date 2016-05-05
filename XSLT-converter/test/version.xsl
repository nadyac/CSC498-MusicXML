<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
	xmlns:msxsl="urn:schemas-microsoft-com:xslt">

	<xsl:output encoding="UTF-8" indent="yes"/>

	<xsl:template match="/">

		<xsl:text>MSXML version: </xsl:text><xsl:value-of select="system-property('msxsl:version')"/>

	</xsl:template>

</xsl:stylesheet>