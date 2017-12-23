package DOMUtilities;


public class DOMChildIterator
{
    private org.w3c.dom.Node m_Node;
    private int m_index;

    public DOMChildIterator(org.w3c.dom.Node i_Node)
    {
	m_Node = i_Node;
	m_index = -1;
    }

    public org.w3c.dom.Node GetNext()
    {
	if (m_Node == null) return null;
	if (!m_Node.hasChildNodes()) return null;
	m_index++;
	if (m_index >= m_Node.getChildNodes().getLength()) return null;
	return m_Node.getChildNodes().item(m_index);
    }
}
